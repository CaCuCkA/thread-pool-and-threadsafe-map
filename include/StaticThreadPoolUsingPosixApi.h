#ifndef THREAD_POOLS_STATIC_THREAD_POOL_USING_POSIX_API_H
#define THREAD_POOLS_STATIC_THREAD_POOL_USING_POSIX_API_H

#include <future>
#include <vector>
#include <unistd.h>
#include <pthread.h>

#include "FunctionWrapper.h"
#include "ThreadSafeQueue.h"

class StaticThreadPoolUsingPosixApi
{
public:
    StaticThreadPoolUsingPosixApi(const StaticThreadPoolUsingPosixApi&) = delete;
    StaticThreadPoolUsingPosixApi& operator=(const StaticThreadPoolUsingPosixApi&) = delete;
    StaticThreadPoolUsingPosixApi(StaticThreadPoolUsingPosixApi&&) = delete;
    StaticThreadPoolUsingPosixApi& operator=(StaticThreadPoolUsingPosixApi&&) = delete;

    StaticThreadPoolUsingPosixApi()
    {
        uint16_t threadsCount = sysconf(_SC_NPROCESSORS_CONF);

        pthread_mutex_init(&m_mutex, nullptr);

        for (uint16_t i = 0; i < threadsCount; ++i)
        {
            pthread_t thread;

            if (pthread_create(&thread, nullptr, &StaticThreadPoolUsingPosixApi::WorkerThread, this) != 0)
            {
                CleanupThreads();
                break;
            }

            m_threads.push_back(thread);
        }
    }

    ~StaticThreadPoolUsingPosixApi()
    {
        CleanupThreads();
        pthread_mutex_destroy(&m_mutex);
    }

    template<typename FunctionType>
    std::future<typename  std::result_of<FunctionType()>::type>
    Submit(FunctionType function)
    {
        typedef typename std::result_of<FunctionType()>::type resultType;
        std::packaged_task<resultType()> task(std::move(function));
        std::future<resultType> result(task.get_future());
        m_workers.Enque(std::move(task));
        return result;
    }

private:
    static void* WorkerThread(void* arg)
    {
        auto* pool = static_cast<StaticThreadPoolUsingPosixApi*>(arg);

        while (!pool->m_done)
        {
            FunctionWrapper task;
            if (pool->m_workers.TryDeque(task))
            {
                task();
            }
            else
            {
                sched_yield();
            }
        }

        return nullptr;
    }

    void CleanupThreads()
    {
        m_done = true;

        for (pthread_t thread : m_threads)
        {
            pthread_join(thread, nullptr);
        }

        m_threads.clear();
    }

private:
    bool m_done = false;
    pthread_mutex_t m_mutex{};
    std::vector<pthread_t> m_threads;
    ThreadSafeQueue<FunctionWrapper> m_workers;
};

#endif // THREAD_POOLS_STATIC_THREAD_POOL_USING_POSIX_API_H
