#include <iostream>
#include "MprpcApp.h"
#include "user.pb.h"



int main(int argc, char **argv)
{
    MprpcApp::Init(argc, argv); // 初始化框架，读取配置文件

    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    // rpc方法的请求参数
    fixbug::LoginRequest request;
    request.set_name("chen maoyin");
    request.set_pwd("123456");
    
    // rpc方法的响应
    fixbug::LoginResponse response;
    
    // 发起rpc方法的调用 同步阻塞的rpc调用
    MprpcController* controller = new MprpcController(); 
    stub.Login(controller, &request, &response, nullptr);

    // 一次rpc调用完成，读调用的结果
    if(controller->Failed())
    {
        std::cout << controller->ErrorText() << std::endl;
    }
    else
    {
        if(response.result().errcode() == 0)
        {
            std::cout << "rpc login response: " << response.success() << std::endl;
        }
        else
        {
            std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
        }
    }
    

    return 0;
}