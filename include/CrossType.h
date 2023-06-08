#ifndef THREAD_POOLS_CROSS_TYPE_H
#define THREAD_POOLS_CROSS_TYPE_H

#if defined(THREAD_POOL)
#include "StaticThreadPool.h"
#elif defined(QUEUE_THREAD_POOL)
#include "StaticThreadPoolWithLocalQueue.h"
#elif defined(STEALING_THREAD_POOL)
#include "StaticThreadPoolWithWorkStealing.h"
#elif !defined(THREAD_POOL) && !defined(STEALING_THREAD_POOL) && !defined(QUEUE_THREAD_POOL)
#ifdef _WIN32
#include "StaticThreadPoolUsingWinApi.h"
#elif __linux__
#include "StaticThreadPoolUsingPosixApi.h"
#else
#error "Cannot detect OS"
#endif //_WIN32
#endif //defined(THREAD_POOL)

namespace cross_type
{
#if defined(THREAD_POOL)
    typedef StaticThreadPool thread_pool;
#elif defined(QUEUE_THREAD_POOL)
    typedef StaticThreadPoolWithLocalQueue thread_pool;
#elif defined(STEALING_THREAD_POOL)
    typedef StaticThreadPoolWithWorkingStealing thread_pool;
#elif !defined(THREAD_POOL) && !defined(STEALING_THREAD_POOL) && !defined(QUEUE_THREAD_POOL)
#ifdef _WIN32
    typedef StaticThreadPoolUsingWinApi thread_pool;
#else
    typedef StaticThreadPoolUsingPosixApi thread_pool;
#endif //WIN32
#endif //defined(THREAD_POOL)
}
#endif //THREAD_POOLS_CROSS_TYPE_H
