#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <cmath>
#include <set>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "station_client.h"

using google::protobuf::RepeatedField;
using grpc::Channel;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using namespace std;

#define MAX_SIZE 1000

struct Oil {
    string uid, receiver_id;
    long long money = 0;
    RepeatedField<int> route;
    string  msg;

    Oil() {}
    Oil(long long _money, string _uid, string _receiver_id, string _msg, RepeatedField<int> _route) {
        money = _money;
        uid = _uid;
        receiver_id = _receiver_id;
        msg = _msg;
        route = _route;
    }
};

mutex no_money_mtx; // mutex for `no_money`, `used`, `next_oil_id`
mutex port_mtx;     // mutex for `port`, `port_cost`

int port = 0;
map<int, int> port_cost;

int used[MAX_SIZE] = {};
Oil no_money[MAX_SIZE];
int next_oil_id = 0;


class MainStationClient {
    private:
    unique_ptr<MainStation::Stub> stub_;
    
    public:
    MainStationClient() {
        shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:8888", grpc::InsecureChannelCredentials());
        stub_ = MainStation::NewStub(channel);
    }

    void Passed(string uid, string receiver_id, int money) {
        ClientContext context;
        PassedRequest req;
        None resp;
        {
        lock_guard<mutex> _(port_mtx);
        req.set_uid(uid);
        req.set_receiver_id(receiver_id);
        req.set_station_id(port);
        req.set_money(money);
        }
        set_deadline(context);
        stub_->Passed(&context, req, &resp);
    }

    void NoMoney(string uid, string receiver_id, int oil_id, int money) {
        ClientContext context;
        NoMoneyRequest req;
        None resp;
        {
        lock_guard<mutex> _(port_mtx);
        req.set_uid(uid);
        req.set_receiver_id(receiver_id);
        req.set_station_id(port);
        req.set_oil_id(oil_id);
        req.set_money(money);
        }
        set_deadline(context);
        stub_->NoMoney(&context, req, &resp);
    }

    void GetOil(string uid, string msg, string receiver_id) {
        ClientContext context;
        GetOilRequest req;
        None resp;
        {
        lock_guard<mutex> _(port_mtx);
        req.set_uid(uid);
        req.set_msg(msg);
        req.set_receiver_id(receiver_id);
        req.set_station_id(port);
        }
        set_deadline(context);
        stub_->GetOil(&context, req, &resp);
    }

    void Fail(int station_id) {
        ClientContext context;
        FailRequest req;
        None resp;
        req.set_station_id(station_id);
        set_deadline(context);
        stub_->Fail(&context, req, &resp);
    }
};


class StationImpl final : public Station::Service {
    private:
    MainStationClient main_client;

    void send_oil(Oil oil) {
        if (oil.money < 0) {
            int oil_id = 0;
            {
            lock_guard<mutex> _(no_money_mtx);
            oil_id = next_oil_id++;
            next_oil_id %= MAX_SIZE;
            no_money[oil_id] = oil;
            used[oil_id] = 1;
            }
            main_client.NoMoney(oil.uid, oil.receiver_id, oil_id, oil.money);
        }
        else if (oil.route.size() > 1) {
            int next = oil.route[oil.route.size() - 2];
            main_client.Passed(oil.uid, oil.receiver_id, oil.money);
            grpc::Status resp = StationClient(next).SendOil(oil.uid, oil.receiver_id, oil.msg, oil.money, oil.route);
            if (!resp.ok() && resp.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
                main_client.Fail(next);
            }
            return;
        }
        else {
            main_client.GetOil(oil.uid, oil.msg, oil.receiver_id);
            return;
        }
    }

    public:
    grpc::Status Init(ServerContext *, const InitRequest *req, None *) override {
        lock_guard<mutex> _(port_mtx);
        port = req->port();
        return grpc::Status::OK;
    }

    grpc::Status Link(ServerContext *, const LinkRequest *req, None*) override {
        lock_guard<mutex> _(port_mtx);
        int _port = req->port();
        int _cost = req->cost();
        port_cost[_port] = _cost;
        return grpc::Status::OK;
    }

    grpc::Status SendOil(ServerContext *, const SendOilRequest *req, None *) override {
        int prev = req->route()[req->route().size() - 1];
        RepeatedField<int> route(req->route().begin(), req->route().end());
        int money = req->money();
        {
            lock_guard<mutex> _(port_mtx);
            if (prev != port) {
                money -= port_cost.at(prev);
                route.RemoveLast();
            }
        }
        Oil oil(money, req->uid(), req->receiver_id(), req->msg(), route);
        send_oil(oil);
        return grpc::Status::OK;
    }

    grpc::Status AddMoney(ServerContext *, const AddMoneyRequest *req, None *) override {
        int oil_id = req->oil_id();
        Oil to_send;
        {
            lock_guard<mutex> _(no_money_mtx);
            if (!used[oil_id]) {
                return grpc::Status::OK;
            }
            no_money[oil_id].money += req->amount();
            used[oil_id] = 0;
            to_send = no_money[oil_id];
        }
        send_oil(to_send);
        return grpc::Status::OK;
    }
};

int main(int, char **argv, char **) {
    string port = argv[1];
    StationImpl service;
    ServerBuilder builder;
    builder.AddListeningPort("127.0.0.1:"+port, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    unique_ptr<Server> server(builder.BuildAndStart());
    server->Wait();
    return 0;
}