#ifndef THREAD_POOL_AND_THREADSAFE_MAP_STATIC_THREAD_POOL_WITH_LOCAL_QUEUE_H
#define THREAD_POOL_AND_THREADSAFE_MAP_STATIC_THREAD_POOL_WITH_LOCAL_QUEUE_H

#include <memory>
#include <queue>
#include <future>
#include <thread>
#include <atomic>
#include <vector>

#include "JoinThreads.h"
#include "FunctionWrapper.h"
#include "ThreadSafeQueue.h"

class StaticThreadPoolWithLocalQueue
{
public:
    StaticThreadPoolWithLocalQueue(const StaticThreadPoolWithLocalQueue&) = delete;
    StaticThreadPoolWithLocalQueue& operator=(const StaticThreadPoolWithLocalQueue&) = delete;
    StaticThreadPoolWithLocalQueue(StaticThreadPoolWithLocalQueue&&) = delete;
    StaticThreadPoolWithLocalQueue& operator=(StaticThreadPoolWithLocalQueue&&) = delete;

    StaticThreadPoolWithLocalQueue()
        : m_done(false), m_joiner(m_threads)
    {
        const uint16_t threadsCount = std::thread::hardware_concurrency();
        try
        {
            for (uint16_t i = 0; i < threadsCount; ++i)
            {
                m_threads.emplace_back(&StaticThreadPoolWithLocalQueue::WorkerThread, this);
            }
        }
        catch (...)
        {
            m_done = true;
            throw;
        }
    }

    ~StaticThreadPoolWithLocalQueue()
    {
        m_done = true;
    }

    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type>
    Submit(FunctionType function)
    {
        typedef typename std::result_of<FunctionType()>::type resultType;
        std::packaged_task<resultType()> task(std::move(function));
        std::future<resultType> result(task.get_future());

        if (m_localQueue)
        {
            m_localQueue->push(std::move(task));
        }
        else
        {
            m_mainQueue.Enque(std::move(task));
        }

        return result;
    }

private:
    void RunPendingTask()
    {
        FunctionWrapper task;

        if (m_localQueue && !m_localQueue->empty())
        {
            task = std::move(m_localQueue->front());
            m_localQueue->pop();
            task();
        }
        else if (m_mainQueue.TryDeque(task))
        {
            task();
        }
        else
        {
            std::this_thread::yield();
        }
    }

    void WorkerThread()
    {
        m_localQueue = std::make_unique<local_queue_type>();

        while (!m_done)
        {
            RunPendingTask();
        }
    }

private:
    std::atomic_bool m_done;
    ThreadSafeQueue<FunctionWrapper> m_mainQueue;
    typedef std::queue<FunctionWrapper> local_queue_type;
    static thread_local std::unique_ptr<local_queue_type> m_localQueue;
    std::vector<std::thread> m_threads;
    JoinThreads m_joiner;
};

thread_local std::unique_ptr<StaticThreadPoolWithLocalQueue::local_queue_type> StaticThreadPoolWithLocalQueue::m_localQueue;

#endif //THREAD_POOL_AND_THREADSAFE_MAP_STATIC_THREAD_POOL_WITH_LOCAL_QUEUE_H
