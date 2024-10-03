#include "MprpcApp.h"
#include <iostream>
#include <unistd.h>

// 定义静态成员变量
MprpcApp* MprpcApp::app = nullptr;
std::mutex MprpcApp::mtx;
MprpcConfig MprpcApp::m_config;


MprpcApp* MprpcApp::GetInstance()
{
    if(app == nullptr)
    {
        std::lock_guard<std::mutex> lock(mtx);
        if(app = nullptr)
        {
            app = new MprpcApp();
        }  
    }
    return app;
}

void ShowArgsHelp()
{
    std::cout << "format: command -i <configfile>" << std::endl;
}

void MprpcApp::Init(int argc, char **argv)
{
    if(argc < 2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }
    
    int c = 0;
    std::string config_file;
    while((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            std::cout << "invalid args!" << std::endl;
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            std::cout << "need <configfile>" << std::endl;
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        
        default:
            break;
        }
    }

    m_config.LoadConfigFile(config_file.c_str());

    // std::cout << "rpcserver_ip:" << m_config.Load("rpcserver_ip") << std::endl;
    // std::cout << "rpcserver_port:" << m_config.Load("rpcserver_port") << std::endl;
    // std::cout << "zookeeper_ip:" << m_config.Load("zookeeper_ip") << std::endl;
    // std::cout << "zookeeper_port:" << m_config.Load("zookeeper_port") << std::endl;

}

MprpcConfig* MprpcApp::GetConfig()
{
    return &m_config;
}