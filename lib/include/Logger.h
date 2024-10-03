#pragma once

#include "LockQueue.h"

enum LogLevel
{
    INFO,
    ERROR,
};

// Mprpc框架提供的日志系统
class Logger
{
public:
    void SetLogLevel(LogLevel level); // 设置日志级别
    void Log(std::string msg); // 写日志
    static Logger* GetInstance();

private:
    int m_loglevel; // 记录日志级别
    LockQueue<std::string> m_lq; // 日志缓冲队列


    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger operator=(const Logger&) = delete;
    Logger operator=(Logger&&) = delete;
    static Logger* instance;
    static std::mutex mtx;
};

// 定义宏
#define LOG_INFO(logmsgformat, ...) \
    do \
    { \
        Logger *logger = Logger::GetInstance(); \
        logger->SetLogLevel(INFO); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger->Log(c); \
    }while(0);


#define LOG_ERR(logmsgformat, ...) \
    do \
    { \
        Logger *logger = Logger::GetInstance(); \
        logger->SetLogLevel(ERROR); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger->Log(c); \
    }while(0);