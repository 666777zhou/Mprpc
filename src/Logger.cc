#include "Logger.h"
#include <time.h>
#include <iostream>

Logger* Logger::instance = nullptr;
std::mutex Logger::mtx;

// 传统双检测锁实现单例模式
Logger* Logger::GetInstance()
{
    if(instance == nullptr)
    {
        std::lock_guard<std::mutex> lock(mtx);
        if(instance == nullptr)
        {
            instance = new Logger();
        }
    }
    return instance;
}

// C++11以后，可通过static局部变量实现线程安全的单例模式
// Logger* Logger::GetInstance()
// {
//     static Logger log;
//     return &log;
// }

Logger::Logger()
{
    // 启动写日志线程
    std::thread writeLogTask([&]()
        {
            for(;;)
            {
                // 获取当前日期，取日志信息，写入相应日志文件中
                time_t now = time(nullptr);
                tm* now_tm = localtime(&now);
                
                char file_name[128];
                sprintf(file_name, "%d-%d-%d-log.txt", now_tm->tm_year+1900, now_tm->tm_mon+1, now_tm->tm_mday);

                FILE* pf = fopen(file_name, "a+");
                if(pf == nullptr)
                {
                    std::cout << "logger file : " << file_name << " open error!" <<std::endl;
                    exit(EXIT_FAILURE);
                }

                std::string msg = m_lq.Pop();

                char time_buf[128] = {0};
                sprintf(time_buf, "%d:%d:%d =>[%s] ", 
                        now_tm->tm_hour, 
                        now_tm->tm_min, 
                        now_tm->tm_sec,
                        m_loglevel == INFO ? "INFO" : "ERROR");
                msg.insert(0, time_buf);
                msg.append("\n");

                fputs(msg.c_str(), pf);
                fclose(pf);
            }
        }
    );
    writeLogTask.detach();
}

void Logger::SetLogLevel(LogLevel level)
{
    m_loglevel = level;
}

void Logger::Log(std::string msg)
{
    m_lq.Push(msg);
}
