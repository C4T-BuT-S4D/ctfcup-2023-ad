#include <grpcpp/grpcpp.h>

#include "station.grpc.pb.h"
#include "station.grpc.pb.cc"
#include "station.pb.h"
#include "station.pb.cc"

using google::protobuf::RepeatedField;
using grpc::Channel;
using grpc::ClientContext;
using namespace std;

class StationClient {
    private:
    unique_ptr<Station::Stub> stub_;

    public:
    StationClient(int port) {
        shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:"+to_string(port), grpc::InsecureChannelCredentials());
        stub_ = Station::NewStub(channel);
    }

    void InitStation(int port, int x, int y) {
        ClientContext context;
        InitRequest req;
        None resp;
        req.set_port(port);
        req.set_x(x);
        req.set_y(y);
        
        stub_->Init(&context, req, &resp);
    }

    void LinkStation(int port, int cost) {
        ClientContext context;
        LinkRequest req;
        None resp;
        req.set_port(port);
        req.set_cost(cost);

        stub_->Link(&context, req, &resp);
    }

    void SendOil(string uid, string receiver_id, string msg, int money, RepeatedField<int> &route) {
        ClientContext context;
        SendOilRequest req;
        None resp;
        req.set_uid(uid);
        req.set_receiver_id(receiver_id);
        req.set_msg(msg);
        req.set_money(money);
        req.mutable_route()->Swap(&route);

        stub_->SendOil(&context, req, &resp);
    }

    void AddMoney(int amount, int oil_id) {
        ClientContext context;
        AddMoneyRequest req;
        None resp;
        req.set_amount(amount);
        req.set_oil_id(oil_id);

        stub_->AddMoney(&context, req, &resp);
    }
};
