#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>


// 异步日志队列 模板类方法的实现只能在头文件中
template<typename T>
class LockQueue
{
public:
    // 生产者
    void Push(const T &data)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_queue.push(data);
        m_cv.notify_one(); // 不会释放锁，通知其他线程来竞争锁
    }

    // 消费者
    T Pop()
    {
        std::unique_lock<std::mutex> lock(m_mtx);

        while(m_queue.empty())
        {
            // 日志队列为空，线程进入wait状态
            m_cv.wait(lock); // 线程等会儿再来，释放互斥量，允许Push方法获取锁、添加数据
        }

        T data = m_queue.front();
        m_queue.pop();
        return data;
    }
    
private:
    std::queue<T> m_queue;
    std::mutex m_mtx;
    std::condition_variable m_cv;
};