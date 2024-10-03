#include <iostream>
#include <string>
#include "user.pb.h"
#include "MprpcApp.h"
#include "MprpcProvider.h"
#include "Logger.h"


class UserService : public fixbug::UserServiceRpc // rpc服务提供者
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "doing local service : Login" << std::endl;
        std::cout << "name:" << name << " " << "pwd:" << pwd << std::endl;
        return true;
    }

    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done)
    {
        // 框架给业务上报请求参数LoginRequest，获取数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 做本地业务
        bool login_result = Login(name, pwd);

        // 写入响应
        fixbug::ResultCode* code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        // 执行回调操作 执行响应对象的序列化和网络发送
        done->Run();
    }
};

int main(int argc, char **argv)
{
    LOG_INFO("first log message!");
    LOG_ERR("%s:%s:%d", __FILE__, __FUNCTION__, __LINE__);

    // 初始化框架
    MprpcApp::Init(argc, argv);

    // provider是一个rpc网络服务对象，用于把UserService对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new UserService());

    // 启动一个rpc服务发布节点，Run以后进程进入阻塞状态，等待远程调用
    provider.Run();

    return 0;
}