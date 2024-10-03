#pragma once

#include "MprpcConfig.h"
#include "MprpcChannel.h"
#include "MprpcController.h"
#include <mutex>
#include "Logger.h"
#include "ZooKeeperUtil.h"

// mprpc框架的初始化类 设计为单例
class MprpcApp
{
public:
    static void Init(int argc, char **argv);
    static MprpcApp* GetInstance();
    static MprpcConfig* GetConfig();
    
private:
    static MprpcConfig m_config;

    MprpcApp(){}
    MprpcApp(const MprpcApp&) = delete;
    MprpcApp(MprpcApp&&) = delete;
    MprpcApp operator=(const MprpcApp&) = delete;
    MprpcApp operator=(MprpcApp&&) = delete;
    static MprpcApp* app;
    static std::mutex mtx;
};