#include "pmRpcApplication.h"
#include <iostream>
#include <unistd.h>
#include <string>

PmRpcConfig pmRpcApplication::m_config;

void ShowArgsHelp()
{
    std::cout << "format: command -i <configfile>" << std::endl;
}

pmRpcApplication::pmRpcApplication()
{
}

pmRpcApplication *pmRpcApplication::getInstance()
{
    static pmRpcApplication pm;
    return &pm;
}

void pmRpcApplication::init(int argc, char **argv)
{
    if (argc < 2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    //配置文件名
    std::string config_file;

    while ((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    // 开始加载配置文件了 rpcserver_ip=  rpcserver_port   zookeeper_ip=  zookepper_port=
    m_config.LoadConfigFile(config_file.c_str());
}

PmRpcConfig &pmRpcApplication::GetConfig()
{
    return m_config;
}