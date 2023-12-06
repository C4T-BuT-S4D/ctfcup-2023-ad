#include "crow/crow_all.h"
#include <pqxx/pqxx>
#include <asio.hpp>
#include <grpcpp/grpcpp.h>

#include <queue>
#include <set>
#include <cmath>

#include <arpa/inet.h>
#include <fcntl.h>
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

class App final : public MainStation::Service {
    private:
    crow::SimpleApp http_server;

    map<int, vector<int>> stations;
    map<int, pair<int, int>> port_coords;
    map<int, int> pid_port, port_pid;
    mutex port_mtx; // mutex for `stations`, `port_coords`, `pid_port`, `port_pid`

    map<crow::websocket::connection*, string> connection_user;
    map<string, set<crow::websocket::connection*>> user_connections;
    mutex conn_mtx; // mutes for `connection_user`, `user_connections`

    int get_distance_sqr(pair<int, int> p1, pair<int, int> p2) {
        return ((p1.first-p2.first)*(p1.first-p2.first)+(p1.second-p2.second)*(p1.second-p2.second));
    }

    int get_cost(int port1, int port2) {
        int d = get_distance_sqr(port_coords.at(port1), port_coords.at(port2));
        int cost = ceil(sqrt(d) * 1);
        return cost;
    }

    int start_station(int port) {
        pid_t child_pid;
        string exe = "/app/stations/" + to_string(port);
        system(("cp /app/station " + exe).c_str());
        child_pid = fork();
        if (child_pid == 0) {
            char *argv[] = {(char*)exe.c_str(), (char*)to_string(port).c_str(), NULL};
            execve(exe.c_str(), argv, 0);
            _exit(0);
        }
        usleep(100000);
        shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:"+to_string(port), grpc::InsecureChannelCredentials());
        StationClient(channel).InitStation(port);
        return child_pid;
    }

    void link_stations(int port1, int port2) {
        lock_guard<mutex> _(port_mtx);
        int cost = get_cost(port1, port2);
        StationClient(port1).LinkStation(port2, cost);
        StationClient(port2).LinkStation(port1, cost);
        stations[port1].push_back(port2);
        stations[port2].push_back(port1);
    }

    bool port_exists(int port) {
        lock_guard<mutex> _(port_mtx);
        return port_coords.find(port) != port_coords.end();
    }

    void restart_station(int port) {
        lock_guard<mutex> _(port_mtx);
        int old_pid = port_pid[port];
        kill(old_pid, SIGKILL);
        int new_pid = start_station(port);
        pid_port.erase(old_pid);
        pid_port[new_pid] = port;
        port_pid[port] = new_pid;
        for (auto port2 : stations.at(port)) {
            StationClient(port).LinkStation(port2, get_cost(port, port2));
        }
    }

    string rand_hex() {
        int fd = open("/dev/urandom", O_RDONLY);
        char buf[0x10] = {};
        read(fd, &buf, 0x10);
        close(fd);
        char hex[0x21] = {};
        for (int i = 0; i < (int)sizeof(buf); ++i) {
            sprintf(&hex[i*2], "%02hhx", buf[i]);
        }
        return string(hex);
    }

    bool check_cred(string cred) {
        if (!(8 <= cred.size() && cred.size() <= 32)) {
            return false;
        }
        for (auto c : cred) {
            if (!(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9')))
                return false;
        }
        return true;
    }

    vector<int> calculate_route(int from, int to) {
        map<int, int> d, p;
        set<pair<int, int>> q;
        d[from] = 0;
        q.insert({0, from});
        while (!q.empty()) {
            auto [l, v] = *q.begin();
            q.erase(q.begin());
            lock_guard<mutex> _(port_mtx);
            for (auto u : stations.at(v)) {
                int dist = get_distance_sqr(port_coords.at(v), port_coords.at(u));
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

    crow::response RegisterUser(const crow::request &req) {
        try {
            pqxx::work w(connection);

            auto json = crow::json::load(req.body);
            if (!json || !json.has("username") || !json.has("password")) {
                return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
            }
            string username = string(json["username"].s());
            string password = string(json["password"].s());

            if (!check_cred(username)) return crow::response(crow::status::UNAUTHORIZED, "{\"error\":\"Username doesn't match regex /^[a-zA-Z0-9]{8,32}$/\"}");
            if (!check_cred(password)) return crow::response(crow::status::UNAUTHORIZED, "{\"error\":\"Password doesn't match regex /^[a-zA-Z0-9]{8,32}$/\"}");

            auto r = w.exec_params("select 1 from users where username=$1", username);
            if (!r.empty()) {
                return crow::response(crow::status::UNAUTHORIZED, "{\"error\":\"User exists\"}");
            }

            string id = rand_hex();

            w.exec_params("insert into users (id, username, password, balance) values ($1, $2, $3, 500)", id, username, password);
            w.commit();

            crow::json::wvalue resp;
            resp["id"] = id;
            return crow::response(crow::status::OK, resp);
        }
        catch (std::runtime_error&) {
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
        catch (std::runtime_error&) {
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
        catch (std::runtime_error&) {
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
        catch (std::runtime_error&) {
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
        catch (std::runtime_error&) {
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

            if (!port_exists(from)) {
                return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid `from`\"}");
            }
            if (!port_exists(to)) {
                return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid `to`\"}");
            }
            if (from == to) {
                return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"`from` mustn't be equal `to`\"}");
            }

            vector<int> route_vec = calculate_route(from, to);

            crow::json::wvalue::list res;
            for (auto station: route_vec) {
                res.push_back(station);
            }

            return crow::response(crow::status::OK, crow::json::wvalue(res));
        }
        catch (std::runtime_error&) {
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

            auto [user_balance] = *w.query<int>("select balance from users where id="+w.quote(uid)).begin();
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
            if (!port_exists(from)) {
                return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid source\"}");
            }
            if (!port_exists(to)) {
                return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid destination\"}");
            }
            if (from == to) {
                return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"`from` mustn't be equal `to`\"}");
            }

            user_balance -= money;
            w.exec_params("update users set balance=$2 where id=$1", uid, user_balance);
            w.commit();

            vector<int> route_vec = calculate_route(from, to);
            if (route_vec.size() == 0) {
                return crow::response(crow::status::OK, "{\"error\":\"Stations aren't linked\"}");
            }

            RepeatedField<int> route(route_vec.begin(), route_vec.end());
            StationClient(from).SendOil(uid, receiver_id, msg, money, route);

            return crow::response(crow::status::OK, "{\"data\":\"ok\"}");
        }
        catch (std::runtime_error&) {
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

            auto [user_balance] = *w.query<int>("select balance from users where id="+w.quote(uid)).begin();
            auto amount = json["amount"].i();
            auto station_id = json["station_id"].i();
            auto oil_id = json["oil_id"].i();
            if (amount > user_balance) {
                return crow::response(crow::status::OK, "{\"error\":\"Invalid amount\"}");
            }
            if (!port_exists(station_id)) {
                return crow::response(crow::status::OK, "{\"error\":\"Invalid station_id\"}");
            }
            user_balance -= amount;
            w.exec_params("update users set balance=$2 where id=$1", uid, user_balance);
            w.commit();

            grpc::Status resp = StationClient(station_id).AddMoney(amount, oil_id);
            if (!resp.ok() && resp.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
                restart_station(station_id);
            }
            return crow::response(crow::status::OK, "{\"data\":\"ok\"}");
        }
        catch (std::runtime_error&) {
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
        catch (std::runtime_error&) {
                return crow::response(crow::status::BAD_REQUEST, "{\"error\":\"Invalid json\"}");
        }
    }

    void set_http_routes() {
        CROW_ROUTE(http_server, "/register").methods(crow::HTTPMethod::Post)(
            [&](const crow::request &req){
                return RegisterUser(req);
            });

        CROW_ROUTE(http_server, "/login").methods(crow::HTTPMethod::Post)(
            [&](const crow::request &req){
                return LoginUser(req);
            });

        CROW_ROUTE(http_server, "/user").methods(crow::HTTPMethod::Post)(
            [&](const crow::request &req){
                return GetUser(req);
            });

        CROW_ROUTE(http_server, "/stations").methods(crow::HTTPMethod::Post)(
            [&](const crow::request &req){
                return ListStations(req);
            });

        CROW_ROUTE(http_server, "/links").methods(crow::HTTPMethod::Post)(
            [&](const crow::request &req){
                return ListLinks(req);
            });

        CROW_ROUTE(http_server, "/route").methods(crow::HTTPMethod::Post)(
            [&](const crow::request &req){
                return CalcRoute(req);
            });

        CROW_ROUTE(http_server, "/send").methods(crow::HTTPMethod::Post)(
            [&](const crow::request &req) {
                return SendOil(req);
            });
        
        CROW_ROUTE(http_server, "/add_money").methods(crow::HTTPMethod::Post)(
            [&](const crow::request &req) {
                return AddMoney(req);
            });

        CROW_ROUTE(http_server, "/last_received").methods(crow::HTTPMethod::Post)(
            [&](const crow::request &req) {
                return LastReceived(req);
            });
    }

    void set_websocket_routes() {
        CROW_WEBSOCKET_ROUTE(http_server, "/ws")
        .onclose([&](crow::websocket::connection &conn, const string &) {
            lock_guard<mutex> _(conn_mtx);
            if (connection_user.find(&conn) != connection_user.end()) {
                string uid = connection_user[&conn];
                user_connections[uid].erase(&conn);
                connection_user.erase(&conn);
            }
        })
        .onmessage([&](crow::websocket::connection &conn, const string &data, bool) {
            try {
                lock_guard<mutex> _(conn_mtx);
                if (connection_user.find(&conn) != connection_user.end()) {
                    return;
                }
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
            catch (std::runtime_error&) {
                conn.send_text("{\"error\":\"Invalid json\"}");
                return;
            }
        });
    }

    public:
    App() {
        pqxx::work w(connection);

        for (auto [port, x, y] : w.query<int, int, int>("select port, x, y from stations")) {
            int pid = start_station(port);
            {
                lock_guard<mutex> _(port_mtx);
                pid_port[pid] = port;
                port_pid[port] = pid;
                port_coords[port] = {x, y};
            }
        }
        for (auto [port1, port2] : w.query<int, int>("select port1, port2 from links")) {
            link_stations(port1, port2);
        }

        set_http_routes();
        set_websocket_routes();
    }

    ~App() {
        lock_guard<mutex> _(port_mtx);
        for (auto [pid, port] : pid_port) {
            kill(pid, SIGKILL);
        }
        system("rm -rf stations/*");
    }

    grpc::Status Passed(ServerContext *, const PassedRequest *req, None *) override {
        lock_guard<mutex> _(conn_mtx);
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
        lock_guard<mutex> _(conn_mtx);
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
        auto resp = crow::json::wvalue();
        resp["type"] = "get";
        resp["receiver_id"] = req->receiver_id();
        resp["station_id"] = req->station_id();
        {
            lock_guard<mutex> _(conn_mtx);
            for (auto conn : user_connections[req->uid()]) {
                conn->send_text(resp.dump());
            }
        }
        
        pqxx::work w(connection);
        struct timeval now;
        gettimeofday(&now, NULL);
        long long time = now.tv_sec * 1000000 + now.tv_usec;
        w.exec_params("INSERT INTO oil (id, sender_id, receiver_id, message, station_id, time) VALUES ($1, $2, $3, $4, $5, $6)", rand_hex(), req->uid(), req->receiver_id(), req->msg(), req->station_id(), time);
        w.commit();

        return grpc::Status::OK;
    }

    grpc::Status Fail(ServerContext *, const FailRequest *req, None *) override {
        restart_station(req->station_id());
        return grpc::Status::OK;
    }

    void healthcheck() {
        int status;
        pid_t wpid;
        while (1) {
            wpid = waitpid(-1, &status, 0);
            if (wpid > 0 && WIFSIGNALED(status) && !(WTERMSIG(status) == SIGKILL || WTERMSIG(status) == SIGINT)) {
                lock_guard<mutex> _(port_mtx);
                int port = pid_port.at(wpid);
                int new_pid = start_station(port);
                pid_port.erase(wpid);
                pid_port[new_pid] = port;
                port_pid[port] = new_pid;
                for (auto port2 : stations.at(port)) {
                    StationClient(port).LinkStation(port2, get_cost(port, port2));
                }
            }
        }
    }

    void runHttpServer() {
        http_server.loglevel(crow::LogLevel::Critical);
        http_server.bindaddr("0.0.0.0").port(8087).run();
    }

    void runGrpcServer() {
        ServerBuilder builder;
        builder.AddListeningPort("127.0.0.1:8888", grpc::InsecureServerCredentials());
        builder.RegisterService(this);
        unique_ptr<Server> server(builder.BuildAndStart());
        server->Wait();
    }

};

int main() {
    App app;
    thread grpc_server(&App::runGrpcServer, &app);
    thread healthcheck(&App::healthcheck, &app);
    app.runHttpServer();

    return 0;
}