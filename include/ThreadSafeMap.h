#ifndef THREADSAFE_ARRAY_H
#define THREADSAFE_ARRAY_H

#include <unordered_map>
#include <mutex>
#include <shared_mutex>

template<typename Key, typename Value>
class ThreadSafeMap
{
public:
    ThreadSafeMap() = default;
    ~ThreadSafeMap() = default;
    ThreadSafeMap(const ThreadSafeMap&) = delete;
    ThreadSafeMap& operator=(const ThreadSafeMap&) = delete;
    ThreadSafeMap(ThreadSafeMap&&) = delete;
    ThreadSafeMap& operator=(ThreadSafeMap&&) = delete;

    void Insert(const Key& key, const Value& value)
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_map.insert(std::make_pair(key, value));
    }

    void Remove(const Key& key)
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_map.erase(key);
    }

    bool Find(const Key& key, Value& value) const
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        auto it = m_map.find(key);
        if (it != m_map.end())
        {
            value = it->second;
            return true;
        }
        return false;
    }

    Value& operator[](const Key& key)
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        return m_map[key];
    }

    size_t Size() const
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return m_map.size();
    }

    bool Empty()
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return m_map.empty();
    }

private:
    std::unordered_map<Key, Value> m_map;
    mutable std::shared_mutex m_mutex;
};
#endif //THREADSAFE_ARRAY_H
