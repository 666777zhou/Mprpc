#include "ZooKeeperUtil.h"
#include "MprpcApp.h"
#include <iostream>


// 全局的watcher观察器
void global_watcher(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx)
{
    if(type == ZOO_SESSION_EVENT)
    {
        if(state == ZOO_CONNECTED_STATE) // zkclient和zkserver连接成功
        {
            sem_t* sem = (sem_t*)zoo_get_context(zh);
            sem_post(sem); // 信号量+1
        }
    }
}

ZkClient::ZkClient() : m_zhandle(nullptr){}

ZkClient::~ZkClient()
{
    if(m_zhandle != nullptr)
    {
        zookeeper_close(m_zhandle); // 关闭句柄，释放资源
    }
}

// 连接zk服务端
void ZkClient::Start()
{
    std::string host = MprpcApp::GetInstance()->GetConfig()->Load("zookeeper_ip");
    std::string port = MprpcApp::GetInstance()->GetConfig()->Load("zookeeper_port");
    std::string connstr = host + ":" + port;

    /*
    zookeeper_mt:多线程版本
    zookeeper的API客户端程序提供了三个线程
    API调用线程
    网络I/O线程
    watcher回调线程
    */
   m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
   if(m_zhandle == nullptr)
   { 
        LOG_ERR("zookeeper_init error!");
        exit(EXIT_FAILURE);
   }

   sem_t sem;
   sem_init(&sem, 0, 0);
   zoo_set_context(m_zhandle, &sem);

   sem_wait(&sem); // 通过回调函数使信号量加1后结束阻塞
   LOG_INFO("zookeeper_init success!");
}

// 创建znode节点
void ZkClient::Create(const char* path, const char* data, int datalen, int state)
{
    char path_buf[128];
    int buf_len = sizeof(path_buf);
    int flag;
    // 先判断path表示的znode节点是否存在，如果存在就不重复创建
    flag = zoo_exists(m_zhandle, path, 0, nullptr);
    if(flag == ZNONODE)
    {
        flag = zoo_create(m_zhandle, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE, state, path_buf, buf_len);
        if(flag == ZOK)
        {
            LOG_INFO("znode create success... path: %s", path);
        }
        else
        {
            LOG_ERR("flag: %d", flag);
            LOG_ERR("znode create error... path: %s", path);
            exit(EXIT_FAILURE); 
        }
    }
}

// 根据指定的path，获取znode节点的值
std::string ZkClient::GetData(const char* path)
{
    char buffer[64];
    int buffer_len = sizeof(buffer);
    int flag = zoo_get(m_zhandle, path, 0, buffer, &buffer_len, nullptr);
    if(flag != ZOK)
    {
        LOG_ERR("get znode error... path: %s", path);
        return "";
    }
    else
    {
        return buffer;
    }
}
