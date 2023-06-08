#ifndef THREAD_POOLS_WORK_STEALING_QUEUE_H
#define THREAD_POOLS_WORK_STEALING_QUEUE_H

#include <deque>
#include <mutex>

template<typename T>
class WorkStealingQueue
{
public:
    WorkStealingQueue() = default;
    ~WorkStealingQueue() = default;
    WorkStealingQueue(const WorkStealingQueue&) = delete;
    WorkStealingQueue& operator=(const WorkStealingQueue&) = delete;
    WorkStealingQueue(WorkStealingQueue&&) = delete;
    WorkStealingQueue& operator=(WorkStealingQueue&&) = delete;

    void Enque(T data)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_buffer.push_front(std::move(data));
    }

    bool TryDeque(T& data)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_buffer.empty())
        {
            return false;
        }

        data = std::move(m_buffer.front());
        m_buffer.pop_front();
        return true;
    }

    bool TrySteal(T& data)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_buffer.empty())
        {
            return false;
        }
        data = std::move(m_buffer.back());
        m_buffer.pop_back();
        return true;
    }

private:
    std::deque<T> m_buffer;
    mutable std::mutex m_mutex;
};

#endif //THREAD_POOLS_WORK_STEALING_QUEUE_H
