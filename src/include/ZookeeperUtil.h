#pragma once

#include <zookeeper/zookeeper.h>
#include <semaphore.h>
#include <string>

#include "pmRpcApplication.h"

class ZkClient
{
private:
    zhandle_t *m_zhandle = nullptr;

public:
    ZkClient();
    ~ZkClient();
    // zkclient connect zkserver
    void Start();

    //给定path创建节点
    //永久性节点和临时性节点区别: 临时性节点再client和server连接超时后，zkserver会主动删除超时节点
    void Create(const char *path, const char *data,
                int datalen, int state = ZOO_EPHEMERAL /*默认为临时性节点 state=0为永久性节点*/);
    //通过path向zkserver查询指定节点的数据
    std::string GetData(const char *path);
};
