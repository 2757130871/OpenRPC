
#include "RpcProvider.h"
#include "pmRpcApplication.h"
#include "ZookeeperUtil.h"
#include "rpcheader.pb.h"

RpcProvider::RpcProvider()
{
}

//框架暴露出来的用来发布RPC方法的接口
void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo service_info;
    //获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor *serviceDesc = service->GetDescriptor();

    //获取服务的名字
    const std::string &service_name = serviceDesc->name();
    //获取RPC方法的数量
    int method_count = serviceDesc->method_count();

    std::cout << "service_name: " << service_name << std::endl;
    //获取一个RPC对象中的所有RPC方法
    for (int i = 0; i < method_count; i++)
    {
        const google::protobuf::MethodDescriptor *methodDesc = serviceDesc->method(i);
        const std::string &method_name = methodDesc->name();
        service_info.m_methodMap.insert({method_name, methodDesc});
        // methodDesc->
        std::cout << "mothed_name: " << method_name << std::endl;
    }

    service_info.m_service = service;
    //存储service服务对象
    m_serviceMap.insert({service_name, service_info});
}

//启动RPC服务节点 开始提供RPC远程调用服务
void RpcProvider::Run()
{
    std::string s1 = "rpcserverip";
    std::string s2 = "rpcserverport";
    std::string ip = pmRpcApplication::getInstance()->GetConfig().Load(s1);
    uint16_t port = atoi(pmRpcApplication::getInstance()->GetConfig().Load(s2).c_str());

    muduo::net::InetAddress addr("ip", port);

    //创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop, addr, "RpcProvider");

    //绑定连接回调
    server.setConnectionCallback(std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));
    //绑定消息读写回调
    server.setMessageCallback(
        std::bind(&RpcProvider::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    //设置工作线程数量
    server.setThreadNum(4);

    // 把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
    // session timeout   30s     zkclient 网络I/O线程  1/3 * timeout 时间发送ping消息
    ZkClient zkCli;
    zkCli.Start();
    // service_name为永久性节点    method_name为临时性节点
    for (auto &sp : m_serviceMap)
    {
        // /service_name   /UserServiceRpc
        std::string service_path = "/" + sp.first;
        zkCli.Create(service_path.c_str(), nullptr, 0, 0);
        for (auto &mp : sp.second.m_methodMap)
        {
            // /service_name/method_name   /UserServiceRpc/Login 存储当前这个rpc服务节点主机的ip和port
            std::string method_path = service_path + "/" + mp.first;

            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            // ZOO_EPHEMERAL表示znode是一个临时性节点
            zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
            std::cout << "DEBUG: ZOO_EPHEMERAL: " << ZOO_EPHEMERAL << std::endl;
            std::cout << "DEBUG: ZOO_SEQUENCE: " << ZOO_SEQUENCE << std::endl;
        }
    }

    //开始监听
    server.start();
    //阻塞等待连接
    m_eventLoop.loop();
}

void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr &conn)
{
    // RPC请求基于短链接 请求一次就断开
    if (conn->disconnected())
    {
        conn->shutdown();
    }
}

void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr &conn,
                            muduo::net::Buffer *buffer,
                            muduo::Timestamp)

{
    // 网络上接收的远程rpc调用请求的字符流    Login args
    std::string recv_buf = buffer->retrieveAllAsString();

    // 从字符流中读取前4个字节的内容
    uint32_t header_size = 0;
    recv_buf.copy((char *)&header_size, 4, 0);

    // 根据header_size读取数据头的原始字符流，反序列化数据，得到rpc请求的详细信息
    std::string rpc_header_str = recv_buf.substr(4, header_size);

    pmrpc::RpcHeader rpcHeader;

    std::string service_name; //请求的服务名
    std::string method_name;  //请求的方法名
    uint32_t args_size;       //后续参数大小

    if (rpcHeader.ParseFromString(rpc_header_str))
    {
        // 数据头反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.arg_size();
    }
    else
    {
        // 数据头反序列化失败
        std::cout << "rpc_header_str:" << rpc_header_str << " parse error!" << std::endl;
        return;
    }

    // 获取rpc方法参数的字符流数据
    std::string args_str = recv_buf.substr(4 + header_size, args_size);

    // 打印调试信息
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "============================================" << std::endl;

    // 获取service对象和method对象
    auto it = m_serviceMap.find(service_name);
    if (it == m_serviceMap.end())
    {
        std::cout << service_name << " is not exist!" << std::endl;
        return;
    }

    // key: service_name val: ServiceInfo {Service*, method_map}
    auto mit = it->second.m_methodMap.find(method_name);
    if (mit == it->second.m_methodMap.end())
    {
        std::cout << service_name << ":" << method_name << " is not exist!" << std::endl;
        return;
    }

    google::protobuf::Service *service = it->second.m_service;      //获取Service对象 UserService
    const google::protobuf::MethodDescriptor *method = mit->second; //获取method对象    Login

    //生成RPC请求的request对象和response对象
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    //从args_str中反序列化出request对象
    if (!request->ParseFromString(args_str))
    {
        std::cout << service_name << "request ParseFromString Error!" << std::endl;
        return;
    }

    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    //在框架上调用当前RPC节点指定的方法

    class Done : public google::protobuf::Closure
    {
    public:
        Done(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *resp) : conn_(conn), resp_(resp) {}
        void sendRpcResponse()
        {
            std::string send_str;
            if (resp_->SerializeToString(&send_str)) // response进行序列化
            {
                // 序列化成功后，通过网络把rpc方法执行的结果发送会rpc的调用方
                conn_->send(send_str);
            }
            else
            {
                std::cout << "response SerializeToString Error!" << std::endl;
            }
            //短连接
            conn_->shutdown();
        }
        void Run()
        {
            sendRpcResponse();
        }

        const muduo::net::TcpConnectionPtr &conn_;
        google::protobuf::Message *resp_;
    };

    Done done(conn, response);

    //给Closure绑定一个回调(序列化 + 发送回RPC的消费者)
    // google::protobuf::Closure *done =
    //     google::protobuf::NewCallback<RpcProvider,
    //                                   const muduo::net::TcpConnectionPtr &,
    //                                   google::protobuf::Message *>(this, &RpcProvider::sendRpcResponse, conn, response);

    //相当于 new UserService().Login(controller, request, response, done)
    service->CallMethod(method, nullptr, request, response, &done);
}

// Closure绑定此方法 用于RPC请求处理结果的序列化的网络发送
void RpcProvider::sendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response)
{
}