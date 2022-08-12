
#include <iostream>
#include "User.pb.h"

#include "pmRpcApplication.h"
#include "RpcProvider.h"

using namespace std;

class UserService : public cxr::UserServiceRpc
{
public:
    //这个代表实际的业务方法
    bool Login(const string &name, const string &pwd)
    {
        cout << "name: " << name << "  logined" << endl;
        return false;
    }

    bool Register_Local(const string &name, const string &pwd)
    {
        cout << "注册成功: " << name << endl;
        return true;
    }

    void Login(::google::protobuf::RpcController *controller,
               const ::cxr::LoginRequest *request,
               ::cxr::LoginResponse *response,
               ::google::protobuf::Closure *done)
    {
        // 框架给业务上报了请求参数LoginRequest，应用获取相应数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 做本地业务
        bool login_result = Login(name, pwd);

        // 把响应写入  包括错误码、错误消息、返回值
        cxr::ResultCode *code = response->mutable_res_code();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        // 执行回调操作   执行响应对象数据的序列化和网络发送（都是由框架来完成的）
        done->Run();
    }

    void Register(::google::protobuf::RpcController *controller,
                  const ::cxr::RegisterRequest *request,
                  ::cxr::RegisterResponse *response,
                  ::google::protobuf::Closure *done)
    {
        // 框架给业务上报了请求参数LoginRequest，应用获取相应数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 做本地业务
        bool res = Register_Local(name, pwd);

        // 把响应写入  包括错误码、错误消息、返回值
        cxr::ResultCode *code = response->mutable_res_code();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(res);

        // 执行回调操作   执行响应对象数据的序列化和网络发送（都是由框架来完成的）
        done->Run();
    }
};

int main(int argc, char **argv)
{
    //初始化RPC框架
    pmRpcApplication::init(argc, argv);

    //网络服务提供对象 将UserService发布到节点上
    RpcProvider provider;
    provider.NotifyService(new UserService());

    //阻塞 等待RPC消费者请求
    provider.Run();
}
