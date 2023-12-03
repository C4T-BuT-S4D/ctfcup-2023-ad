#include "crow/crow_all.h"
#include <pqxx/pqxx>
#include <asio.hpp>
#include <grpcpp/grpcpp.h>

#include <queue>
#include <set>
#include <cmath>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "station_client.h"

using google::protobuf::RepeatedField;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using namespace std;

thread_local pqxx::connection connection(getenv("PG_DSN"));
map<int, vector<int>> stations;
map<int, pair<int, int>> port_coords;
map<int, int> pid_port;

void startStation(int port, int x, int y) {
    pid_t child_pid;
    string exe = "/app/stations/" + to_string(port);
    
    system(("cp /app/station " + exe).c_str());

    child_pid = fork();
    if (child_pid == 0) {
        char *argv[] = {(char*)exe.c_str(), (char*)to_string(port).c_str(), NULL};
        execve(exe.c_str(), argv, 0);
        _exit(0);
    }
    pid_port[child_pid] = port;
    port_coords[port] = {x, y};
    usleep(50000);
    StationClient(port).InitStation(port, x, y);
}


int getDistanceSqr(pair<int, int> p1, pair<int, int> p2) {
    return ((p1.first-p2.first)*(p1.first-p2.first)+(p1.second-p2.second)*(p1.second-p2.second));
}

void linkStations(int port1, int port2) {
    int d = getDistanceSqr(port_coords[port1], port_coords[port2]);
    int cost = ceil(sqrt(d) * 1);
    StationClient(port1).LinkStation(port2, cost);
    StationClient(port2).LinkStation(port1, cost);
    stations[port1].push_back(port2);
    stations[port2].push_back(port1);
}

void loadStations() {
    pqxx::work w(connection);

    for (auto [port, x, y] : w.query<int, int, int>("select port, x, y from stations")) {
        startStation(port, x, y);
    }
    for (auto [port1, port2] : w.query<int, int>("select port1, port2 from links")) {
        linkStations(port1, port2);
    }
}

string generateId() {
    FILE* rand = fopen("/dev/urandom", "rb");
    char buf[0x10];
    fread(buf, sizeof(buf), 1, rand);
    fclose(rand);
    char hex[0x21] = {};
    for (int i = 0; i < (int)sizeof(buf); ++i) {
        sprintf(&hex[i*2], "%02hhx", buf[i]);
    }
    return string(hex);
}

bool checkCred(string cred) {
    if (!(8 <= cred.size() && cred.size() <= 32)) {
        return false;
    }
    for (auto c : cred) {
        if (!(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9')))
            return false;
    }
    return true;
}

vector<int> calculateRoute(int from, int to) {
    map<int, int> d;
    map<int, int> p;
    set<pair<int, int>> q;
    d[from] = 0;
    q.insert({0, from});
    while (!q.empty()) {
        auto [l, v] = *q.begin();
        q.erase(q.begin());
        for (auto u : stations[v]) {
            int dist = getDistanceSqr(port_coords[v], port_coords[u]);
            if (d.find(u) == d.end() || l + dist < d[u]) {
                d[u] = l + dist;
                p[u] = v;
                q.insert(make_pair(d[u], u));
            }
        }
    }
    if (d.find(to) == d.end()) return {};
    vector<int> route;
    int x = to;
    while (x != from) {
        route.push_back(x);
        x = p[x];
    }
    route.push_back(from);
    return route;
}

int getBalance(string uid, pqxx::work &w) {
    auto [user_balance] = *w.query<int>("select balance from users where id="+w.quote(uid)).begin();
    return user_balance;
}

crow::response RegisterUser(const crow::request &req) {
    try {
        pqxx::work w(connection);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("username") || !json.has("password")) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
        }
        string username = string(json["username"].s());
        string password = string(json["password"].s());

        if (!checkCred(username)) return crow::response(crow::status::UNAUTHORIZED, "{\"error\":\"Username doesn't match regex /^[a-zA-Z0-9]{8,32}$/\"}");
        if (!checkCred(password)) return crow::response(crow::status::UNAUTHORIZED, "{\"error\":\"Password doesn't match regex /^[a-zA-Z0-9]{8,32}$/\"}");

        auto r = w.exec_params("select 1 from users where username=$1", username);
        if (!r.empty()) {
            return crow::response(crow::status::UNAUTHORIZED, "{\"error\":\"User exists\"}");
        }

        string id = generateId();

        w.exec_params("insert into users (id, username, password, balance) values ($1, $2, $3, 500)", id, username, password);
        w.commit();

        crow::json::wvalue resp;
        resp["id"] = id;
        return crow::response(crow::status::OK, resp);
    }
    catch (std::runtime_error) {
        return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
    }
}

crow::response LoginUser(const crow::request &req) {
    try {
        pqxx::work w(connection);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("username") || !json.has("password")) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
        }
        string username = string(json["username"].s());
        string password = string(json["password"].s());

        auto r = w.exec_params("select id from users where username=$1 and password=$2", username, password);
        if (r.empty()) {
            return crow::response(crow::status::UNAUTHORIZED, "{\"error\":\"Wrong username or password\"}");
        }

        auto id = string(r[0][0].c_str());

        crow::json::wvalue resp;
        resp["id"] = id;
        return crow::response(crow::status::OK, resp);
    }
    catch (std::runtime_error) {
        return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
    }
}

crow::response GetUser(const crow::request &req) {
    try {
        pqxx::work w(connection);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("uid")) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
        }
        string uid = string(json["uid"].s());
        auto r = w.exec_params("select username, balance from users where id=$1", uid);
        if (r.empty()) {
            return crow::response(crow::status::UNAUTHORIZED, "{\"error\":\"Invalid uid\"}");
        }

        auto [username, balance] = *w.query<string, int>("select username, balance from users where id="+w.quote(uid)).begin();
        crow::json::wvalue res;

        res["username"] = username;
        res["uid"] = uid;
        res["balance"] = balance;
        return crow::response(crow::status::OK, res);
    }
    catch (std::runtime_error) {
        return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
    }
}

crow::response ListStations(const crow::request &req) {
    try {
        pqxx::work w(connection);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("uid")) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
        }
        string uid = string(json["uid"].s());
        auto r = w.exec_params("select id from users where id=$1", uid);
        if (r.empty()) {
            return crow::response(crow::status::UNAUTHORIZED, "{\"error\":\"Invalid uid\"}");
        }

        crow::json::wvalue::list res;
        for (auto [port, x, y] : w.query<int, int, int>("select port, x, y from stations")) {
            crow::json::wvalue station;
            station["id"] = port;
            station["x"] = x;
            station["y"] = y;
            res.push_back(station);
        }

        return crow::response(crow::status::OK, crow::json::wvalue(res));
    }
    catch (std::runtime_error) {
        return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
    }
}

crow::response ListLinks(const crow::request &req) {
    try {
        pqxx::work w(connection);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("uid")) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
        }
        string uid = string(json["uid"].s());
        auto r = w.exec_params("select id from users where id=$1", uid);
        if (r.empty()) {
            return crow::response(crow::status::UNAUTHORIZED, "{\"error\":\"Invalid uid\"}");
        }

        crow::json::wvalue::list res;
        for (auto [port1, port2] : w.query<int, int>("select port1, port2 from links")) {
            crow::json::wvalue link;
            link["id1"] = port1;
            link["id2"] = port2;
            res.push_back(link);
        }
        return crow::response(crow::status::OK, crow::json::wvalue(res));
    }
    catch (std::runtime_error) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
    }
}

crow::response CalcRoute(const crow::request &req) {
    try {
        auto json = crow::json::load(req.body);
        if (!json || !json.has("uid") || !json.has("from") | !json.has("to")) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
        }
        pqxx::work w(connection);

        string uid = json["uid"].s();
        auto r = w.exec_params("select id from users where id=$1", uid);
        if (r.empty()) {
            return crow::response(crow::status::UNAUTHORIZED, "{\"error\":\"Invalid uid\"}");
        }

        auto from = json["from"].i();
        auto to = json["to"].i();

        r = w.exec_params("select port from stations where port=$1", from);
        if (r.empty()) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid `from`\"}");
        }
        r = w.exec_params("select port from stations where port=$1", to);
        if (r.empty()) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid `to`\"}");
        }
        if (from == to) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"`from` mustn't be equal `to`\"}");
        }

        vector<int> route_vec = calculateRoute(from, to);

        crow::json::wvalue::list res;
        for (auto station: route_vec) {
            res.push_back(station);
        }

        return crow::response(crow::status::OK, crow::json::wvalue(res));
    }
    catch (std::runtime_error) {
        return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
    }
}

crow::response SendOil(const crow::request &req) {
    try {
        auto json = crow::json::load(req.body);
        if (!json || !json.has("uid") || !json.has("receiver_id") | !json.has("msg") || !json.has("money") || !json.has("from") || !json.has("to")) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
        }
        pqxx::work w(connection);

        string uid = json["uid"].s();
        auto r = w.exec_params("select balance from users where id=$1", uid);
        if (r.empty()) {
            return crow::response(crow::status::UNAUTHORIZED, "{\"error\":\"Invalid uid\"}");
        }
        string receiver_id = json["receiver_id"].s();
        r = w.exec_params("select id from users where id=$1", receiver_id);
        if (r.empty()) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid receiver_id\"}");
        }

        int user_balance = getBalance(uid, w);

        auto money = json["money"].i();
        auto from = json["from"].i();
        auto to = json["to"].i();
        string msg = json["msg"].s();
        if (money <= 0) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"money must be > 0\"}");
        }
        if (money > user_balance) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"money is too big\"}");
        }
        if (stations.find(from) == stations.end()) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid source\"}");
        }
        if (stations.find(to) == stations.end()) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid destination\"}");
        }
        if (from == to) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"`from` mustn't be equal `to`\"}");
        }

        user_balance -= money;
        w.exec_params("update users set balance=$2 where id=$1", uid, user_balance);
        w.commit();

        vector<int> route_vec = calculateRoute(from, to);
        if (route_vec.size() == 0) {
            return crow::response(crow::status::OK, "{\"error\":\"Stations aren't linked\"}");
        }

        RepeatedField<int> route(route_vec.begin(), route_vec.end());
        StationClient(from).SendOil(uid, receiver_id, msg, money, route);

        return crow::response(crow::status::OK, "{\"data\":\"ok\"}");
    }
    catch (std::runtime_error) {
        return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
    }
}

crow::response AddMoney(const crow::request &req) {
    try {
        auto json = crow::json::load(req.body);
        if (!json || !json.has("uid") || !json.has("amount") || !json.has("station_id") || !json.has("oil_id")) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
        }
        pqxx::work w(connection);

        string uid = json["uid"].s();
        auto r = w.exec_params("select 1 from users where id=$1", uid);
        if (r.empty()) {
            return crow::response(crow::status::UNAUTHORIZED, "{\"error\":\"Invalid uid\"}");
        }

        auto user_balance = getBalance(uid, w);
        auto amount = json["amount"].i();
        auto station_id = json["station_id"].i();
        auto oil_id = json["oil_id"].i();
        if (amount > user_balance) {
            return crow::response(crow::status::OK, "{\"error\":\"Invalid amount\"}");
        }
        if (port_coords.find(station_id) == port_coords.end()) {
            return crow::response(crow::status::OK, "{\"error\":\"Invalid station_id\"}");
        }
        user_balance -= amount;
        w.exec_params("update users set balance=$2 where id=$1", uid, user_balance);
        w.commit();

        StationClient(station_id).AddMoney(amount, oil_id);
        return crow::response(crow::status::OK, "{\"data\":\"ok\"}");
    }
    catch (std::runtime_error) {
        return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
    }
}

crow::response LastReceived(const crow::request &req) {
    try {
        pqxx::work w(connection);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("uid")) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
        }
        string uid = string(json["uid"].s());
        auto r = w.exec_params("select id from users where id=$1", uid);
        if (r.empty()) {
            return crow::response(crow::status::UNAUTHORIZED, "{\"error\":\"Invalid uid\"}");
        }

        crow::json::wvalue::list res;
        for (auto [sender_id, message, station_id] : w.query<string, string, int>("select sender_id, message, station_id from oil where receiver_id="+w.quote(uid)+" order by time desc limit 10")) {
            auto [username] = *w.query<string>("select username from users where id="+w.quote(sender_id)).begin();
            crow::json::wvalue oil;
            oil["sender"] = username;
            oil["message"] = message;
            oil["station_id"] = station_id;
            res.push_back(oil);
        }
        return crow::response(crow::status::OK, crow::json::wvalue(res));
    }
    catch (std::runtime_error) {
            return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
    }
}

map<crow::websocket::connection*, string> connection_user;
map<string, set<crow::websocket::connection*>> user_connections;
mutex mtx;

class MainStationImpl final : public MainStation::Service {
    public:
    grpc::Status Passed(ServerContext *, const PassedRequest *req, None *) override {
        lock_guard<mutex> _(mtx);
        auto resp = crow::json::wvalue();
        resp["type"] = "passed";
        resp["receiver_id"] = req->receiver_id();
        resp["station_id"] = req->station_id();
        resp["money"] = req->money();
        for (auto conn : user_connections[req->uid()]) {
            conn->send_text(resp.dump());
        }
        return grpc::Status::OK;
    }

    grpc::Status NoMoney(ServerContext *, const NoMoneyRequest *req, None *) override {
        lock_guard<mutex> _(mtx);
        auto resp = crow::json::wvalue();
        resp["type"] = "no_money";
        resp["receiver_id"] = req->receiver_id();
        resp["station_id"] = req->station_id();
        resp["oil_id"] = req->oil_id();
        resp["money"] = req->money();
        for (auto conn : user_connections[req->uid()]) {
            conn->send_text(resp.dump());
        }
        return grpc::Status::OK;
    }

    grpc::Status GetOil(ServerContext *, const GetOilRequest *req, None *) override {
        lock_guard<mutex> _(mtx);
        auto resp = crow::json::wvalue();
        resp["type"] = "get";
        resp["receiver_id"] = req->receiver_id();
        resp["station_id"] = req->station_id();
        for (auto conn : user_connections[req->uid()]) {
            conn->send_text(resp.dump());
        }
        
        pqxx::work w(connection);
        struct timeval now;
        gettimeofday(&now, NULL);
        long long time = now.tv_sec * 1000000 + now.tv_usec;
        w.exec_params("INSERT INTO oil (id, sender_id, receiver_id, message, station_id, time) VALUES ($1, $2, $3, $4, $5, $6)", generateId(), req->uid(), req->receiver_id(), req->msg(), req->station_id(), time);
        w.commit();

        return grpc::Status::OK;
    }
};

void *runMainStation(void *) {
    MainStationImpl service;

    ServerBuilder builder;
    builder.AddListeningPort("127.0.0.1:8888", grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    unique_ptr<Server> server(builder.BuildAndStart());
    server->Wait();
    return NULL;
}

void *healthcheck(void *) {
    int status;
    pid_t wpid;
    while (1) {
        wpid = waitpid(-1, &status, 0);
        if (wpid > 0 && WIFSIGNALED(status) && !(WTERMSIG(status) == SIGKILL || WTERMSIG(status) == SIGINT)) {
            int port = pid_port[wpid];
            auto [x, y] = port_coords[port];
            pid_port.erase(wpid);
            startStation(port, x, y);
        }
    }
}

int main() {
    loadStations();
    pthread_t main_station_thread;
    pthread_create(&main_station_thread, NULL, runMainStation, NULL);

    pthread_t healthcheck_thread;
    pthread_create(&healthcheck_thread, NULL, healthcheck, NULL);

    crow::SimpleApp app;

    CROW_ROUTE(app, "/register").methods(crow::HTTPMethod::Post)(
        [](const crow::request &req){
            return RegisterUser(req);
        });

    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::Post)(
        [](const crow::request &req){
            return LoginUser(req);
        });

    CROW_ROUTE(app, "/user").methods(crow::HTTPMethod::Post)(
        [](const crow::request &req){
            return GetUser(req);
        });

    CROW_ROUTE(app, "/stations").methods(crow::HTTPMethod::Post)(
        [](const crow::request &req){
            return ListStations(req);
        });

    CROW_ROUTE(app, "/links").methods(crow::HTTPMethod::Post)(
        [](const crow::request &req){
            return ListLinks(req);
        });

    CROW_ROUTE(app, "/route").methods(crow::HTTPMethod::Post)(
        [](const crow::request &req){
            return CalcRoute(req);
        });

    CROW_ROUTE(app, "/send").methods(crow::HTTPMethod::Post)(
        [](const crow::request &req) {
            return SendOil(req);
        });
    
    CROW_ROUTE(app, "/add_money").methods(crow::HTTPMethod::Post)(
        [](const crow::request &req) {
            return AddMoney(req);
        });

    CROW_ROUTE(app, "/last_received").methods(crow::HTTPMethod::Post)(
        [](const crow::request &req) {
            return LastReceived(req);
        });

    CROW_WEBSOCKET_ROUTE(app, "/ws")
        .onclose([&](crow::websocket::connection &conn, const string &reason){
            lock_guard<mutex> _(mtx);
            if (connection_user.find(&conn) != connection_user.end()) {
                string uid = connection_user[&conn];
                user_connections[uid].erase(&conn);
                connection_user.erase(&conn);
            }
        })
        .onmessage([&](crow::websocket::connection &conn, const string &data, bool /*is_binary*/) {
            lock_guard<mutex> _(mtx);
            try {
                auto json = crow::json::load(data);
                if (!json || !json.has("type") || !json.has("uid")) {
                    conn.send_text("{\"error\":\"Invalid json\"}");
                    return;
                }
                string uid = json["uid"].s();
                if (json["type"].s() == "INIT") {
                    connection_user[&conn] = uid;
                    user_connections[uid].insert(&conn);
                    conn.send_text("{\"data\":\"ok\"}");
                    return;
                }
            }
            catch (std::runtime_error) {
                conn.send_text("{\"error\":\"Invalid json\"}");
                return;
            }
        });

    app.loglevel(crow::LogLevel::Critical);
    app.bindaddr("0.0.0.0").port(8087).run();

    for (auto [pid, port] : pid_port) {
        kill(pid, SIGKILL);
    }
    system("rm -rf stations/*");

    return 0;
}