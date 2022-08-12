#include "pmRpcApplication.h"
#include "ZookeeperUtil.h"

#include <iostream>

int main(int argc, char **argv)
{

    pmRpcApplication::init(argc, argv);
    // zk server
    ZkClient zk;
    zk.Start();

    std::string data = "znode_data";
    zk.Start();
    zk.Create("/UserService", data.c_str(), data.size(), 0);
    // std::cout << "--------------------DEBUG: ZOO" << ZOO_EPHEMERAL
    //           << "-------------------- " << std::endl;

    std::string login_data = "login data";
    zk.Create("/UserService/Login", login_data.c_str(), login_data.size());
    std::string znode_data = zk.GetData("/UserService/Login");
    std::cout << "znode_data: " << znode_data << std::endl;
}