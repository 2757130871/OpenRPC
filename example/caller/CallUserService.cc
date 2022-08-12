#include "pmRpcApplication.h"
#include "pmRpcChannel.h"

#include "User.pb.h"

void test_login()
{
    cxr::UserServiceRpc_Stub stub(new pmRpcChannel());
    cxr::LoginRequest req;
    req.set_name("cxr");
    req.set_pwd("1231123");
    cxr::LoginResponse resp;

    stub.Login(nullptr, &req, &resp, nullptr);

    if (resp.success())
    {
        std::cout << "Login success: " << resp.success() << std::endl;
    }
    else
    {
        std::cout << "login error" << std::endl;
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

    test_register();
}