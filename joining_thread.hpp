#ifndef JOINING_THREAD_HPP
#define JOINING_THREAD_HPP

#include <utility>
#include <thread>

namespace bsl 
{
    class joining_thread
    {
    private:
        std::thread t;

    public:
        joining_thread() noexcept = default;
        template<typename Callable, typename ... Args>
        explicit joining_thread(Callable&& func, Args&& ... args)
            : t{ std::forward<Callable>(func), std::forward<Args>(args) } {}
        explicit joining_thread(std::thread thread) noexcept
            : t{ std::move(thread) } {}

        joining_thread(const joining_thread&) = delete;
        joining_thread& operator=(const joining_thread&) = delete;

        joining_thread(joining_thread&& other) noexcept
            : t{ std::move(other.t) } {}

        joining_thread& operator=(joining_thread&& other) noexcept
        {
            if (t.joinable()) {
                t.join();
            }

            t = std::move(other.t);

            return *this;
        }

        joining_thread& operator=(std::thread thread) noexcept
        {
            if (t.joinable()) {
                t.join();
            }

            t = std::move(thread);

            return *this;
        }

        ~joining_thread()
        {
            if (t.joinable()) {
                t.join();
            }
        }

        void swap(joining_thread& other) noexcept
        {
            t.swap(other.t);
        }

        std::thread::id get_id() const noexcept
        {
            return t.get_id();
        }

        bool joinable() const noexcept
        {
            return t.joinable();
        }

        void join()
        {
            t.join();
        }

        void detach()
        {
            t.detach();
        }

        std::thread& as_thread() noexcept
        {
            return const_cast<std::thread&>(std::as_const(*this).as_thread());
        }

        const std::thread& as_thread() const noexcept
        {
            return t;
        }
    };
}

#endif