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


struct Oil {
    string uid, receiver_id, msg;
    long long money = 0;
    RepeatedField<int> route;

    Oil() {}
    Oil(long long _money, string _uid, string _receiver_id, string _msg, RepeatedField<int> _route) {
        money = _money;
        uid = _uid;
        receiver_id = _receiver_id;
        msg = _msg;
        route = _route;
    }
};

#define MAX_SIZE 1000
int port = 0;
int x = 0, y = 0;
int next_oil_id = 0;
map<int, int> port_cost;
Oil out_of_money[MAX_SIZE];


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
        req.set_uid(uid);
        req.set_receiver_id(receiver_id);
        req.set_station_id(port);
        req.set_money(money);
        stub_->Passed(&context, req, &resp);
    }

    void NoMoney(string uid, string receiver_id, int oil_id, int money) {
        ClientContext context;
        NoMoneyRequest req;
        None resp;
        req.set_uid(uid);
        req.set_receiver_id(receiver_id);
        req.set_station_id(port);
        req.set_oil_id(oil_id);
        req.set_money(money);
        stub_->NoMoney(&context, req, &resp);
    }

    void GetOil(string uid, string msg, string receiver_id) {
        ClientContext context;
        GetOilRequest req;
        None resp;
        req.set_uid(uid);
        req.set_msg(msg);
        req.set_receiver_id(receiver_id);
        req.set_station_id(port);
        stub_->GetOil(&context, req, &resp);
    }
};


class StationImpl final : public Station::Service {
    private:
    void send_oil(Oil oil) {
        if (oil.money < 0) {
            int oil_id = next_oil_id++;
            next_oil_id %= MAX_SIZE;
            out_of_money[oil_id] = oil;

            // if (!removed.empty()) {
            //     oil_id = *removed.begin();
            //     removed.erase(removed.begin());
            //     out_of_money[oil_id] = oil;
            // }
            // else {
            //     oil_id = out_of_money.size();
            //     out_of_money.push_back(oil);
            // }

            MainStationClient().NoMoney(oil.uid, oil.receiver_id, oil_id, oil.money);
        }
        else if (oil.route.size() > 1) {
            int next = oil.route[oil.route.size() - 2];
            MainStationClient().Passed(oil.uid, oil.receiver_id, oil.money);
            StationClient(next).SendOil(oil.uid, oil.receiver_id, oil.msg, oil.money, oil.route);
            return;
        }
        else {
            MainStationClient().GetOil(oil.uid, oil.msg, oil.receiver_id);
            return;
        }
    }

    public:
    grpc::Status Init(ServerContext *, const InitRequest *req, None *) override {
        port = req->port();
        x = req->x();
        y = req->y();
        cout << "port: " << port << "; x: " << x << "; y: " << y << '\n';
        return grpc::Status::OK;
    }

    grpc::Status Link(ServerContext *, const LinkRequest *req, None*) override {
        int _port = req->port();
        int _cost = req->cost();
        port_cost[_port] = _cost;
        cout << "port: " << port << "; _port: " << _port << "; _cost: " << _cost << '\n';
        return grpc::Status::OK;
    }

    grpc::Status SendOil(ServerContext *, const SendOilRequest *req, None *) override {
        int prev = req->route()[req->route().size() - 1];
        RepeatedField<int> route(req->route().begin(), req->route().end());
        int money = req->money();
        if (prev != port) {
            money -= port_cost[prev];
            route.RemoveLast();
        }
        cout << "port: " << port << "; money: " << money << ";\n";
        
        Oil oil(money, req->uid(), req->receiver_id(), req->msg(), route);
        send_oil(oil);

        return grpc::Status::OK;
    }

    grpc::Status AddMoney(ServerContext *, const AddMoneyRequest *req, None *) override {
        int oil_id = req->oil_id();
        // if (removed.find(oil_id) != removed.end()) {
        //     return grpc::Status::OK;
        // }
        // if (out_of_money[oil_id].token != req->token()) {
        //     return grpc::Status::OK;
        // }
        out_of_money[oil_id].money += req->amount();
        send_oil(out_of_money[oil_id]);
        // if (out_of_money[oil_id].money >= 0) {
        //     // removed.insert(oil_id);
        // }

        return grpc::Status::OK;
    }
};

int main(int /*argc*/, char **argv, char **/*env*/) {
    string port = argv[1];

    StationImpl service;

    ServerBuilder builder;
    builder.AddListeningPort("127.0.0.1:"+port, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    unique_ptr<Server> server(builder.BuildAndStart());
    cout << "Server listening on " << port << '\n';

    server->Wait();
    return 0;
}