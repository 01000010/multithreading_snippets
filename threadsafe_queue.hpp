#ifndef THREADSAFE_QUEUE_HPP
#define THREADSAFE_QUEUE_HPP

#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <exception>
#include <condition_variable>

namespace bsl
{
    template<typename T>
    class threadsafe_queue
    {
    private:
        mutable std::mutex m;
        std::queue<T> data_queue;
        std::condition_variable data_condition;

    public:
        threadsafe_queue() = default;

        threadsafe_queue(const threadsafe_queue& other)
        {
            std::lock_guard<std::mutex> lock{ m };

            data_queue = other.data_queue;
        }

        threadsafe_queue& operator=(const threadsafe_queue&) = delete;

        void push(T new_value)
        {
            std::lock_guard<std::mutex> lock{ m };

            data_queue.push(std::move(new_value));
            data_condition.notify_one();
        }

        void wait_and_pop(T& value)
        {
            std::unique_lock<std::mutex> lock{ m };

            data_condition.wait(lock, [this] { return !data_queue.empty(); });
            value = data_queue.front();
            data_queue.pop();
        }

        std::shared_ptr<T> wait_and_pop()
        {
            std::unique_lock<std::mutex> lock{ m };

            data_condition.wait(lock, [this] { return !data_queue.empty(); });

            std::shared_ptr<T> result{ std::make_shared(data_queue.front()) };

            data_queue.pop();

            return result;
        }

        bool try_pop(T& value)
        {
            std::lock_guard<std::mutex> lock{ m };

            if (data_queue.empty()) {
                return false;
            }

            value = data_queue.front();
            data_queue.pop();

            return true;
        }

        std::shared_ptr<T> try_pop()
        {
            std::lock_guard<std::mutex> lock{ m };

            if (data_queue.empty()) {
                return std::shared_ptr<T>{};
            }

            std::shared_ptr<T> result{ std::make_shared<T>(data_queue.front()) };

            data_queue.pop();

            return result;
        }

        bool empty() const noexcept
        {
            std::lock_guard<std::mutex> lock{ m };

            return data_queue.empty();
        }
    };
}

#endif