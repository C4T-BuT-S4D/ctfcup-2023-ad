#include <grpcpp/grpcpp.h>

#include "station.grpc.pb.h"
#include "station.grpc.pb.cc"
#include "station.pb.h"
#include "station.pb.cc"

using google::protobuf::RepeatedField;
using grpc::Channel;
using grpc::ClientContext;
using namespace std;


void set_deadline(ClientContext &context) {
    chrono::system_clock::time_point deadline = 
        chrono::system_clock::now() + chrono::seconds(5);
    context.set_deadline(deadline);
}


class StationClient {
    private:
    unique_ptr<Station::Stub> stub_;

    public:
    StationClient(int port) {
        shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:"+to_string(port), grpc::InsecureChannelCredentials());
        stub_ = Station::NewStub(channel);
    }

    StationClient(shared_ptr<Channel> channel) {
        stub_ = Station::NewStub(channel);
    }

    grpc::Status InitStation(int port) {
        ClientContext context;
        set_deadline(context);
        InitRequest req;
        None resp;
        req.set_port(port);        
        return stub_->Init(&context, req, &resp);
    }

    grpc::Status LinkStation(int port, int cost) {
        ClientContext context;
        set_deadline(context);
        LinkRequest req;
        None resp;
        req.set_port(port);
        req.set_cost(cost);
        return stub_->Link(&context, req, &resp);
    }

    grpc::Status SendOil(string uid, string receiver_id, string msg, int money, RepeatedField<int> &route) {
        ClientContext context;
        set_deadline(context);
        SendOilRequest req;
        None resp;
        req.set_uid(uid);
        req.set_receiver_id(receiver_id);
        req.set_msg(msg);
        req.set_money(money);
        req.mutable_route()->Swap(&route);
        return stub_->SendOil(&context, req, &resp);
    }

    grpc::Status AddMoney(int amount, int oil_id) {
        ClientContext context;
        set_deadline(context);
        AddMoneyRequest req;
        None resp;
        req.set_amount(amount);
        req.set_oil_id(oil_id);
        return stub_->AddMoney(&context, req, &resp);
    }
};
