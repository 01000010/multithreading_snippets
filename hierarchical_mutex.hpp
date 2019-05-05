#ifndef HIERARCHICAL_MUTEX_HPP
#define HIERARCHICAL_MUTEX_HPP

#include <mutex>
#include <exception>
#include <limits>

namespace bsl
{
    class hierarchical_mutex
    {
    private:
        std::mutex internal_mutex;
        const size_t hierarchy_value{};
        size_t previous_hierarchy_value{};
        static inline thread_local size_t this_thread_hierarchy_value{ std::numeric_limits<size_t>::max() };

        void check_for_hierarchy_violation()
        {
            if (this_thread_hierarchy_value <= hierarchy_value) {
                throw std::logic_error{ "Mutex hierarchy violated!" };
            }
        }

        void update_hierarchy_value()
        {
            previous_hierarchy_value = this_thread_hierarchy_value;
            this_thread_hierarchy_value = hierarchy_value;
        }

    public:
        explicit hierarchical_mutex(size_t value) : hierarchy_value{ value } {}

        void lock()
        {
            check_for_hierarchy_violation();
            internal_mutex.lock();
            update_hierarchy_value();
        }

        void unlock()
        {
            if (this_thread_hierarchy_value != hierarchy_value) {
                throw std::logic_error{ "Mutex hierarchy violated!" };
            }

            this_thread_hierarchy_value = previous_hierarchy_value;
            internal_mutex.unlock();
        }

        bool try_lock()
        {
            check_for_hierarchy_violation();

            if (!internal_mutex.try_lock()) {
                return false;
            }

            update_hierarchy_value();

            return true;
        }
    };
}

#endif