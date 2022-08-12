#pragma once

#include "pmRpcConfig.h"
// RPC框架的基础类
class pmRpcApplication
{
private:
    /* data */
    pmRpcApplication();
    pmRpcApplication(const pmRpcApplication &) = delete;
    pmRpcApplication(pmRpcApplication &&) = delete;

public:
    static PmRpcConfig m_config;
    static pmRpcApplication *getInstance();

    static void init(int argc, char **argv);
    static PmRpcConfig &GetConfig();
};
