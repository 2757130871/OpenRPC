#pragma once

#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"

class pmRpcChannel : public google::protobuf::RpcChannel
{
private:
    /* data */
public:
    virtual void CallMethod(const google::protobuf::MethodDescriptor *method,
                            google::protobuf::RpcController *controller, const google::protobuf::Message *request,
                            google::protobuf::Message *response, google::protobuf::Closure *done);
    pmRpcChannel(/* args */);
    ~pmRpcChannel();
};
