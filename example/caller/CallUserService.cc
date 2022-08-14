#include "pmRpcApplication.h"
#include "pmRpcChannel.h"
#include "pmController.h"
#include "User.pb.h"

void test_login()
{
    cxr::UserServiceRpc_Stub stub(new pmRpcChannel());
    cxr::LoginRequest req;
    req.set_name("cxr");
    req.set_pwd("1231123");
    cxr::LoginResponse resp;

    // controller对象的作用：在发起一次RPC调用时并不是一定成功，
    //可能序列化出现错误、网络发送错误、反序泪化错误等等，ctrller对象可以记录错误结果
    //在RPC方法调用返回时就可以通过ctrller对象判断此次RPC调用是否发生错误
    pmController ctrller;
    stub.Login(&ctrller, &req, &resp, nullptr);

    if (!ctrller.Failed())
    {
        if (resp.success())
        {
            std::cout << "Login success: " << resp.success() << std::endl;
        }
        else
        {
            std::cout << "login error" << std::endl;
        }
    }
    else
    {
        std::cout << "stub.Login error! reason: " << ctrller.ErrorText() << std::endl;
    }
}

void test_register()
{
    cxr::UserServiceRpc_Stub stub(new pmRpcChannel());

    cxr::RegisterRequest req;
    req.set_name("chenxinrong");
    req.set_pwd("123cxr");
    cxr::RegisterResponse resp;
    stub.Register(nullptr, &req, &resp, nullptr);

    if (resp.success())
    {
        std::cout << "Register success: " << resp.success() << std::endl;
    }
    else
    {
        std::cout << "Register error: " << resp.success() << std::endl;
    }
}

int main(int argc, char **argv)
{
    //第一步 初始化
    pmRpcApplication::init(argc, argv);

    test_login();
}
