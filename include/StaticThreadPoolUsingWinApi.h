#ifndef THREAD_POOLS_STATIC_THREAD_POOL_USING_WINAPI_H
#define THREAD_POOLS_STATIC_THREAD_POOL_USING_WINAPI_H

#include <future>
#include <vector>
#include <windows.h>

class StaticThreadPoolUsingWinApi
{
public:
    StaticThreadPoolUsingWinApi(const StaticThreadPoolUsingWinApi&) = delete;
    StaticThreadPoolUsingWinApi& operator=(const StaticThreadPoolUsingWinApi&) = delete;
    StaticThreadPoolUsingWinApi(StaticThreadPoolUsingWinApi&&) = delete;
    StaticThreadPoolUsingWinApi& operator=(StaticThreadPoolUsingWinApi&&) = delete;

    StaticThreadPoolUsingWinApi()
    {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);

        DWORD threadCount = sysInfo.dwNumberOfProcessors;
        m_threadPool = CreateThreadpool(nullptr);
        SetThreadpoolThreadMinimum(m_threadPool, threadCount);
    }

    ~StaticThreadPoolUsingWinApi()
    {
        for (auto work : m_works)
        {
            CloseThreadpoolWork(work);
        }
        CloseThreadpool(m_threadPool);
    }

    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> Submit(FunctionType function)
    {
        using resultType = typename std::result_of<FunctionType()>::type;
        std::packaged_task<resultType()> task(std::move(function));
        std::future<resultType> result(task.get_future());

        auto work = CreateThreadpoolWork(ThreadPoolWorkCallback<resultType>,
                                            new std::packaged_task<resultType()>(std::move(task)), nullptr);
        m_works.push_back(work);
        SubmitThreadpoolWork(work);

        return result;
    }

private:
    template<typename ResultType>
    static void CALLBACK ThreadPoolWorkCallback(PTP_CALLBACK_INSTANCE, PVOID Context, PTP_WORK)
    {
        auto task = static_cast<std::packaged_task<ResultType()>*>(Context);
        (*task)();
        delete task;
    }

private:
    std::vector<PTP_WORK> m_works;
    PTP_POOL m_threadPool;
};

#endif // THREAD_POOLS_STATIC_THREAD_POOL_USING_WINAPI_H
