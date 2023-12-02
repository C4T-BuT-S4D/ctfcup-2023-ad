// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: station.proto

#include "station.pb.h"
#include "station.grpc.pb.h"

#include <functional>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/impl/channel_interface.h>
#include <grpcpp/impl/client_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/rpc_service_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/sync_stream.h>

static const char* MainStation_method_names[] = {
  "/MainStation/Passed",
  "/MainStation/NoMoney",
  "/MainStation/GetOil",
};

std::unique_ptr< MainStation::Stub> MainStation::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< MainStation::Stub> stub(new MainStation::Stub(channel, options));
  return stub;
}

MainStation::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_Passed_(MainStation_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_NoMoney_(MainStation_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetOil_(MainStation_method_names[2], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status MainStation::Stub::Passed(::grpc::ClientContext* context, const ::PassedRequest& request, ::None* response) {
  return ::grpc::internal::BlockingUnaryCall< ::PassedRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Passed_, context, request, response);
}

void MainStation::Stub::async::Passed(::grpc::ClientContext* context, const ::PassedRequest* request, ::None* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::PassedRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Passed_, context, request, response, std::move(f));
}

void MainStation::Stub::async::Passed(::grpc::ClientContext* context, const ::PassedRequest* request, ::None* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Passed_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::None>* MainStation::Stub::PrepareAsyncPassedRaw(::grpc::ClientContext* context, const ::PassedRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::None, ::PassedRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Passed_, context, request);
}

::grpc::ClientAsyncResponseReader< ::None>* MainStation::Stub::AsyncPassedRaw(::grpc::ClientContext* context, const ::PassedRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncPassedRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status MainStation::Stub::NoMoney(::grpc::ClientContext* context, const ::NoMoneyRequest& request, ::None* response) {
  return ::grpc::internal::BlockingUnaryCall< ::NoMoneyRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_NoMoney_, context, request, response);
}

void MainStation::Stub::async::NoMoney(::grpc::ClientContext* context, const ::NoMoneyRequest* request, ::None* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::NoMoneyRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_NoMoney_, context, request, response, std::move(f));
}

void MainStation::Stub::async::NoMoney(::grpc::ClientContext* context, const ::NoMoneyRequest* request, ::None* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_NoMoney_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::None>* MainStation::Stub::PrepareAsyncNoMoneyRaw(::grpc::ClientContext* context, const ::NoMoneyRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::None, ::NoMoneyRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_NoMoney_, context, request);
}

::grpc::ClientAsyncResponseReader< ::None>* MainStation::Stub::AsyncNoMoneyRaw(::grpc::ClientContext* context, const ::NoMoneyRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncNoMoneyRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status MainStation::Stub::GetOil(::grpc::ClientContext* context, const ::GetOilRequest& request, ::None* response) {
  return ::grpc::internal::BlockingUnaryCall< ::GetOilRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetOil_, context, request, response);
}

void MainStation::Stub::async::GetOil(::grpc::ClientContext* context, const ::GetOilRequest* request, ::None* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::GetOilRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetOil_, context, request, response, std::move(f));
}

void MainStation::Stub::async::GetOil(::grpc::ClientContext* context, const ::GetOilRequest* request, ::None* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetOil_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::None>* MainStation::Stub::PrepareAsyncGetOilRaw(::grpc::ClientContext* context, const ::GetOilRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::None, ::GetOilRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetOil_, context, request);
}

::grpc::ClientAsyncResponseReader< ::None>* MainStation::Stub::AsyncGetOilRaw(::grpc::ClientContext* context, const ::GetOilRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetOilRaw(context, request, cq);
  result->StartCall();
  return result;
}

MainStation::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      MainStation_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< MainStation::Service, ::PassedRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](MainStation::Service* service,
             ::grpc::ServerContext* ctx,
             const ::PassedRequest* req,
             ::None* resp) {
               return service->Passed(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      MainStation_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< MainStation::Service, ::NoMoneyRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](MainStation::Service* service,
             ::grpc::ServerContext* ctx,
             const ::NoMoneyRequest* req,
             ::None* resp) {
               return service->NoMoney(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      MainStation_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< MainStation::Service, ::GetOilRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](MainStation::Service* service,
             ::grpc::ServerContext* ctx,
             const ::GetOilRequest* req,
             ::None* resp) {
               return service->GetOil(ctx, req, resp);
             }, this)));
}

MainStation::Service::~Service() {
}

::grpc::Status MainStation::Service::Passed(::grpc::ServerContext* context, const ::PassedRequest* request, ::None* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status MainStation::Service::NoMoney(::grpc::ServerContext* context, const ::NoMoneyRequest* request, ::None* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status MainStation::Service::GetOil(::grpc::ServerContext* context, const ::GetOilRequest* request, ::None* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


static const char* Station_method_names[] = {
  "/Station/Init",
  "/Station/Link",
  "/Station/SendOil",
  "/Station/AddMoney",
};

std::unique_ptr< Station::Stub> Station::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< Station::Stub> stub(new Station::Stub(channel, options));
  return stub;
}

Station::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_Init_(Station_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Link_(Station_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_SendOil_(Station_method_names[2], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_AddMoney_(Station_method_names[3], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status Station::Stub::Init(::grpc::ClientContext* context, const ::InitRequest& request, ::None* response) {
  return ::grpc::internal::BlockingUnaryCall< ::InitRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Init_, context, request, response);
}

void Station::Stub::async::Init(::grpc::ClientContext* context, const ::InitRequest* request, ::None* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::InitRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Init_, context, request, response, std::move(f));
}

void Station::Stub::async::Init(::grpc::ClientContext* context, const ::InitRequest* request, ::None* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Init_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::None>* Station::Stub::PrepareAsyncInitRaw(::grpc::ClientContext* context, const ::InitRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::None, ::InitRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Init_, context, request);
}

::grpc::ClientAsyncResponseReader< ::None>* Station::Stub::AsyncInitRaw(::grpc::ClientContext* context, const ::InitRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncInitRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status Station::Stub::Link(::grpc::ClientContext* context, const ::LinkRequest& request, ::None* response) {
  return ::grpc::internal::BlockingUnaryCall< ::LinkRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Link_, context, request, response);
}

void Station::Stub::async::Link(::grpc::ClientContext* context, const ::LinkRequest* request, ::None* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::LinkRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Link_, context, request, response, std::move(f));
}

void Station::Stub::async::Link(::grpc::ClientContext* context, const ::LinkRequest* request, ::None* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Link_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::None>* Station::Stub::PrepareAsyncLinkRaw(::grpc::ClientContext* context, const ::LinkRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::None, ::LinkRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Link_, context, request);
}

::grpc::ClientAsyncResponseReader< ::None>* Station::Stub::AsyncLinkRaw(::grpc::ClientContext* context, const ::LinkRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncLinkRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status Station::Stub::SendOil(::grpc::ClientContext* context, const ::SendOilRequest& request, ::None* response) {
  return ::grpc::internal::BlockingUnaryCall< ::SendOilRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_SendOil_, context, request, response);
}

void Station::Stub::async::SendOil(::grpc::ClientContext* context, const ::SendOilRequest* request, ::None* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::SendOilRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_SendOil_, context, request, response, std::move(f));
}

void Station::Stub::async::SendOil(::grpc::ClientContext* context, const ::SendOilRequest* request, ::None* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_SendOil_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::None>* Station::Stub::PrepareAsyncSendOilRaw(::grpc::ClientContext* context, const ::SendOilRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::None, ::SendOilRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_SendOil_, context, request);
}

::grpc::ClientAsyncResponseReader< ::None>* Station::Stub::AsyncSendOilRaw(::grpc::ClientContext* context, const ::SendOilRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncSendOilRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status Station::Stub::AddMoney(::grpc::ClientContext* context, const ::AddMoneyRequest& request, ::None* response) {
  return ::grpc::internal::BlockingUnaryCall< ::AddMoneyRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_AddMoney_, context, request, response);
}

void Station::Stub::async::AddMoney(::grpc::ClientContext* context, const ::AddMoneyRequest* request, ::None* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::AddMoneyRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_AddMoney_, context, request, response, std::move(f));
}

void Station::Stub::async::AddMoney(::grpc::ClientContext* context, const ::AddMoneyRequest* request, ::None* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_AddMoney_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::None>* Station::Stub::PrepareAsyncAddMoneyRaw(::grpc::ClientContext* context, const ::AddMoneyRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::None, ::AddMoneyRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_AddMoney_, context, request);
}

::grpc::ClientAsyncResponseReader< ::None>* Station::Stub::AsyncAddMoneyRaw(::grpc::ClientContext* context, const ::AddMoneyRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncAddMoneyRaw(context, request, cq);
  result->StartCall();
  return result;
}

Station::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Station_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Station::Service, ::InitRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Station::Service* service,
             ::grpc::ServerContext* ctx,
             const ::InitRequest* req,
             ::None* resp) {
               return service->Init(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Station_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Station::Service, ::LinkRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Station::Service* service,
             ::grpc::ServerContext* ctx,
             const ::LinkRequest* req,
             ::None* resp) {
               return service->Link(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Station_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Station::Service, ::SendOilRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Station::Service* service,
             ::grpc::ServerContext* ctx,
             const ::SendOilRequest* req,
             ::None* resp) {
               return service->SendOil(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Station_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Station::Service, ::AddMoneyRequest, ::None, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Station::Service* service,
             ::grpc::ServerContext* ctx,
             const ::AddMoneyRequest* req,
             ::None* resp) {
               return service->AddMoney(ctx, req, resp);
             }, this)));
}

Station::Service::~Service() {
}

::grpc::Status Station::Service::Init(::grpc::ServerContext* context, const ::InitRequest* request, ::None* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Station::Service::Link(::grpc::ServerContext* context, const ::LinkRequest* request, ::None* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Station::Service::SendOil(::grpc::ServerContext* context, const ::SendOilRequest* request, ::None* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Station::Service::AddMoney(::grpc::ServerContext* context, const ::AddMoneyRequest* request, ::None* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


