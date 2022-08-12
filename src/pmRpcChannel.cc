#include "pmRpcChannel.h"
#include "rpcheader.pb.h"
#include "pmRpcApplication.h"

#include <string>
#include <algorithm>
#include <memory>

#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "ZookeeperUtil.h"

bool IsLittleEndian()
{
    int i = 1;
    if (*(char *)&i == 1)
        return true;
    return false;
}

//所有调用stub类对象 调用RPC方法都会响应到此方法 进行参数的序列化的网络发送
void pmRpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                              google::protobuf::RpcController *controller,
                              const google::protobuf::Message *request,
                              google::protobuf::Message *response,
                              google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *sd = method->service();

    //获取服务名和方法名
    std::string service_name = sd->name();
    std::string method_name = method->name();

    // 获取参数的序列化字符串长度 args_size
    uint32_t args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        controller->SetFailed("serialize request error!");
        return;
    }

    // 定义rpc的请求header
    pmrpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_arg_size(args_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if (rpcHeader.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else
    {
        controller->SetFailed("serialize rpc header error!");
        return;
    }

    // 组织待发送的rpc请求的字符串
    std::string send_rpc_str;

    //统一以小端字节序发送
    uint32_t header_size_tmp = header_size;
    if (!IsLittleEndian())
    {
        char *begin = (char *)&header_size_tmp, *end = (char *)&header_size_tmp + sizeof(header_size_tmp) - 1;
        while (begin < end)
        {
            std::swap(*begin, *end);
            begin++;
            end--;
        }
    }
    send_rpc_str.insert(0, std::string((char *)&header_size_tmp, 4)); // header_size
    send_rpc_str += rpc_header_str;                                   // rpcheader
    send_rpc_str += args_str;                                         // args

    // 打印调试信息
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "============================================" << std::endl;

    //返回RPC服务的ip和port
    std::string zk_ip = pmRpcApplication::GetConfig().Load("zookeeperip");
    int zk_port = atoi(pmRpcApplication::GetConfig().Load("zookeeperport").c_str());

    ZkClient zkcli;
    zkcli.Start();

    //拼接znode节点 /UserServiceRpc/Login
    std::string zk_path = "/" + service_name + "/" + method_name;
    std::cout << "=============DEBUG: zk_path: " << zk_path << "=========================" << std::endl;
    std::string znode_data = zkcli.GetData(zk_path.c_str());

    std::string ip = znode_data.substr(0, znode_data.find_first_of(":"));
    int port = std::atoi(znode_data.substr(znode_data.find_first_of(":") + 1).c_str());

    // 使用tcp编程，完成rpc方法的远程调用
    int clientfd = ::socket(AF_INET, SOCK_STREAM, 0);
    int *p_cfd = new int(clientfd);

    std::shared_ptr<int> p_clientfd(p_cfd, [](int *p_cfd)
                                    {
                                        close(*p_cfd);
                                        delete(p_cfd); });

    if (-1 == clientfd)
    {
        char errtxt[512] = {0};
        sprintf(errtxt, "create socket error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (-1 == connect(clientfd, (sockaddr *)&server_addr, sizeof(server_addr)))
    {
        std::cout << "connect to rpc_server erorr!" << std::endl;
        exit(1);
    }

    //发送RPC请求
    if (-1 == send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0))
    {
        std::cout << "send send_rpc_str erorr!" << std::endl;
        exit(1);
    }

    //接收RPC请求
    char recv_buf[1024];
    int recv_size = 0;
    if (-1 >= (recv_size = recv(clientfd, recv_buf, 1024, 0)))
    {
        std::cout << "recv send_rpc_str erorr!" << std::endl;
        exit(1);
    }

    //反序列化RPC响应
    if (!response->ParseFromArray(recv_buf, recv_size))
    {
        std::cout << "ParseFromArray erorr!" << std::endl;
        exit(1);
    }
}

pmRpcChannel::pmRpcChannel(/* args */)
{
}
pmRpcChannel::~pmRpcChannel()
{

    pmrpc::RpcHeader header;
}
