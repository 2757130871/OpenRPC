
#include "ZookeeperUtil.h"

#include "iostream"

ZkClient::ZkClient() : m_zhandle(nullptr)
{
}

ZkClient::~ZkClient()
{
    if (nullptr == m_zhandle)
    {
        //关闭句柄
        zookeeper_close(m_zhandle);
    }
}

void watcher_callback(zhandle_t *zh, int type,
                      int state, const char *path, void *watcherCtx)
{
    if (type == ZOO_SESSION_EVENT) // 回调的消息类型是和会话相关的消息类型
    {
        if (state == ZOO_CONNECTED_STATE) // zkclient和zkserver连接成功
        {
            sem_t *sem = (sem_t *)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}

// zkclient connect zkserver
void ZkClient::Start()
{
    std::string ip = pmRpcApplication::getInstance()->GetConfig().Load("zookeeperip");
    int port = atoi(pmRpcApplication::getInstance()->GetConfig().Load("zookeeperport").c_str());
    std::string conn_str = ip + ":" + std::to_string(port);

    //此方法返回只说明创建句柄成功 不代表连接zkserver成功
    /*
    zookeeper_mt：多线程版本
    zookeeper的API客户端程序提供了三个线程
    API调用线程 网络I/O线程  pthread_create  poll  watcher回调线程 pthread_create
    */
    m_zhandle = zookeeper_init(conn_str.c_str(), watcher_callback, 30000, nullptr, nullptr, 0);

    if (nullptr == m_zhandle)
    {
        std::cout << "zookeeper_init init error!" << std::endl;
        exit(1);
    }
    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(m_zhandle, &sem);

    sem_wait(&sem);

    std::cout << "connect zkServer success!" << std::endl;
}

//给定path创建节点
void ZkClient::Create(const char *path, const char *data, int datalen, int state /*state=ZOO_EPHEMERAL*/)
{

    //返回所创建节点的新路径
    char path_buffer[128];
    int bufferlen = sizeof(path_buffer);
    int flag;
    // 先判断path表示的znode节点是否存在，如果存在，就不再重复创建
    flag = zoo_exists(m_zhandle, path, 0, nullptr);
    if (ZNONODE == flag) // 表示path的znode节点不存在
    {
        // 创建指定path的znode节点
        flag = zoo_create(m_zhandle, path, data, datalen,
                          &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
        if (flag == ZOK)
        {
            std::cout << "znode create success... path:" << path << std::endl;
            std::cout << "path_buffer: " << path_buffer << std::endl;
        }
        else
        {
            std::cout << "flag:" << flag << std::endl;
            std::cout << "znode create error... path:" << path << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

//通过path向zkserver查询指定节点上存储的值
std::string ZkClient::GetData(const char *path)
{
    char buffer[64];
    int bufferlen = sizeof(buffer);

    int flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);

    if (flag == ZOK)
    {
        return buffer;
    }
    else if (flag == ZNONODE)
    {
        std::cout << "get znode error...: znode is not exist!" << std::endl;
        return "";
    }
    else if (flag == ZNOAUTH)
    {
        std::cout << "get znode error...: permission denied!" << std::endl;
        return "";
    }
    return "";
}