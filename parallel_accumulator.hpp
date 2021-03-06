#ifndef PARALLEL_ACCUMULATOR_HPP
#define PARALLEL_ACCUMULATOR_HPP

#include <thread>
#include <vector>
#include <numeric>

namespace bsl
{
    template<typename Iterator, typename T>
    class accumulate_block
    {
    public:
        void operator()(Iterator first, Iterator last, T& result)
        {
            result = std::accumulate(first, last, result);
        }
    };

    template<typename Iterator, typename T>
    T parallel_accumulate(Iterator first, Iterator last, T init)
    {
        const unsigned long length{ static_cast<unsigned long>(std::distance(first, last)) };

        if (!length) {
            return init;
        }

        const unsigned long min_per_thread{ 25 };
        const unsigned long max_threads{ (length + min_per_thread - 1) / min_per_thread };
        const unsigned long hardware_threads{ std::thread::hardware_concurrency() };
        const unsigned long num_threads{ std::min(hardware_threads ? hardware_threads : 2, max_threads) };
        const unsigned long block_size{ length / num_threads };
        std::vector<T> results(num_threads);
        std::vector<std::thread> threads(num_threads - 1);
        Iterator block_start{ first };

        for (unsigned long i{}; i < (num_threads - 1); ++i) {
            Iterator block_end{ block_start };

            std::advance(block_end, block_size);
            threads[i] = std::thread{ accumulate_block<Iterator, T>(), block_start, block_end, std::ref(results[i]) };
            block_start = block_end;
        }

        accumulate_block<Iterator, T>()(block_start, last, results[num_threads - 1]);

        for (std::thread& thread : threads) {
            thread.join();
        }

        return std::accumulate(results.begin(), results.end(), init);
    }
}

#endif