#ifndef THREAD_POOLS_UTILS_H
#define THREAD_POOLS_UTILS_H

#include <random>
#include <thread>
#include <chrono>
#include <atomic>
#include <iostream>
#include <functional>

static std::random_device rd;
static std::mt19937 mt(rd());
static std::uniform_int_distribution<int> dist(-1000, 1000);
static auto rnd = std::bind(dist, mt);

inline void SimulateHardComputation()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(200 + rnd())) ;
}

int Multiply(const int a, const int b)
{
    SimulateHardComputation();
    const int res = a * b;
    return res;
}

inline std::chrono::time_point<std::chrono::high_resolution_clock> getCurrentTime()
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto timeResult = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return timeResult;
}

template <class D>
inline long long toUs(const D& duration)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

#endif //THREAD_POOLS_UTILS_H
