#ifndef THREAD_POOLS_STATIC_THREAD_POOL_WITH_WORK_STEALING_H
#define THREAD_POOLS_STATIC_THREAD_POOL_WITH_WORK_STEALING_H

#include <atomic>
#include <future>
#include <memory>
#include <vector>
#include <thread>
#include <memory>

#include "JoinThreads.h"
#include "FunctionWrapper.h"
#include "ThreadSafeQueue.h"
#include "WorkStealingQueue.h"

class StaticThreadPoolWithWorkingStealing
{
public:
    StaticThreadPoolWithWorkingStealing(const StaticThreadPoolWithWorkingStealing&) = delete;
    StaticThreadPoolWithWorkingStealing& operator=(const StaticThreadPoolWithWorkingStealing&) = delete;
    StaticThreadPoolWithWorkingStealing(StaticThreadPoolWithWorkingStealing&&) = delete;
    StaticThreadPoolWithWorkingStealing& operator=(StaticThreadPoolWithWorkingStealing&&) = delete;

    StaticThreadPoolWithWorkingStealing()
        : m_done(false), m_joiner(m_threads)
    {
        const uint16_t threadsCount = std::thread::hardware_concurrency();

        try
        {
            for (uint16_t i = 0; i < threadsCount; ++i)
            {
                m_queues.push_back(
                        std::make_unique<WorkStealingQueue<FunctionWrapper>>());
            }
            for (uint16_t i = 0; i < threadsCount; ++i)
            {
                m_threads.emplace_back(&StaticThreadPoolWithWorkingStealing::WorkerThread, this, i);
            }
        }
        catch (...)
        {
            m_done = true;
            throw;
        }
    }

    ~StaticThreadPoolWithWorkingStealing()
    {
        m_done = true;
    }

    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type>
    Submit(FunctionType function)
    {
        typedef typename std::result_of<FunctionType()>::type resultType;
        std::packaged_task<resultType()> task(function);
        std::future<resultType> result(task.get_future());

        if (m_localQueue)
        {
            m_localQueue->Enque(std::move(task));
        }
        else
        {
            m_mainQueue.Enque(std::move(task));
        }

        return result;
    }

private:
    void WorkerThread(uint16_t t_myIndex)
    {
        m_myIndex = t_myIndex;
        m_localQueue = m_queues[m_myIndex].get();

        while (!m_done)
        {
            RunPendingTask();
        }
    }

    void RunPendingTask()
    {
        FunctionWrapper task;

        if (PopTaskFromLocalQueue(task) ||
            PopTaskFromPoolQueue(task) ||
            PopTaskFromOtherThreadQueue(task))
        {
            task();
        }
        else
        {
            std::this_thread::yield();
        }
    }

    static inline bool PopTaskFromLocalQueue(FunctionWrapper& task)
    {
        return m_localQueue && m_localQueue->TryDeque(task);
    }

    inline bool PopTaskFromPoolQueue(FunctionWrapper& task)
    {
        return m_mainQueue.TryDeque(task);
    }

    inline bool PopTaskFromOtherThreadQueue(FunctionWrapper& task)
    {
        for (size_t i = 0; i < m_queues.size(); ++i)
        {
            const uint16_t index = (m_myIndex + i + 1) % m_queues.size();

            if (m_queues[index]->TrySteal(task))
            {
                return true;
            }
        }

        return false;
    }

private:
    std::atomic_bool m_done;
    ThreadSafeQueue<FunctionWrapper> m_mainQueue;
    std::vector<std::unique_ptr<WorkStealingQueue<FunctionWrapper>>> m_queues;
    std::vector<std::thread> m_threads;
    JoinThreads m_joiner;
    static thread_local WorkStealingQueue<FunctionWrapper>* m_localQueue;
    static thread_local uint16_t m_myIndex;
};

thread_local WorkStealingQueue<FunctionWrapper>* StaticThreadPoolWithWorkingStealing::m_localQueue;
thread_local uint16_t StaticThreadPoolWithWorkingStealing::m_myIndex;

#endif //THREAD_POOLS_STATIC_THREAD_POOL_WITH_WORK_STEALING_H
