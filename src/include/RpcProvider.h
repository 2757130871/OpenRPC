#pragma once

#include "google/protobuf/service.h"
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>

#include <muduo/net/TcpConnection.h>

#include <string>
#include <functional>
#include <unordered_map>

#include "google/protobuf/descriptor.h"
#include "google/protobuf/service.h"

class RpcProvider
{
private:
    /* data */
public:
    RpcProvider(/* args */);

    //框架暴露出来的用来发布RPC方法的接口
    void NotifyService(google::protobuf::Service *service);

    //启动RPC服务节点 开始提供RPC远程调用服务
    void Run();

    void onConnection(const muduo::net::TcpConnectionPtr &conn);
    void onMessage(const muduo::net::TcpConnectionPtr &conn,
                   muduo::net::Buffer *buffer,
                   muduo::Timestamp);
    // Closure绑定此方法 用于RPC请求处理结果的序列化的网络发送
    void sendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response);

private:
    muduo::net::EventLoop m_eventLoop;

    //服务类型信息
    struct ServiceInfo
    {
        google::protobuf::Service *m_service;                                                    //保存服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor *> m_methodMap; //保存服务方法
    };

    //存储已注册的服务对象和服务方法所有信息
    std::unordered_map<std::string, ServiceInfo> m_serviceMap;
};
