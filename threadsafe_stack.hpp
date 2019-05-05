#ifndef THREADSAFE_STACK_HPP
#define THREADSAFE_STACK_HPP

#include <memory>
#include <stack>
#include <mutex>
#include <exception>

namespace bsl 
{
    struct empty_stack : std::exception
    {
        const char* what() const throw();
    };

    template<typename T>
    class threadsafe_stack
    {
    private:
        mutable std::mutex m;
        std::stack<T> data;

    public:
        threadsafe_stack() = default;

        threadsafe_stack(const threadsafe_stack& other)
        {
            // Copy is performed inside the function body to ensure the mutex is held.
            std::lock_guard<std::mutex> lock{ m };

            data = other.data;
        }

        threadsafe_stack& operator=(const threadsafe_stack&) = delete;

        void swap(threadsafe_stack& other)
        {
            if (this == &other) {
                return;
            }

            std::scoped_lock<std::mutex> lock{ m, other.m };
            
            std::swap(data, other.data);
        }

        void push(T new_value)
        {
            std::lock_guard<std::mutex> lock{ m };

            data.push(std::move(new_value));
        }

        std::shared_ptr<T> pop()
        {
            std::lock_guard<std::mutex> lock{ m };

            if (data.empty()) {
                throw empty_stack();
            }

            const std::shared_ptr<T> result{ std::make_shared<T>(data.top()) };

            data.pop();

            return res;
        }

        void pop(T& value)
        {
            std::lock_guard<std::mutex> lock{ m };

            if (data.empty()) {
                throw empty_stack();
            }

            value = data.top();
            data.pop();
        }

        bool empty() const
        {
            std::lock_guard<std::mutex> lock{ m };

            return data.empty();
        }
    };
}

#endif