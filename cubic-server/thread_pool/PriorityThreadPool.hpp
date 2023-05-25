#ifndef ZENITH_PRIORITYTHREADPOOL_HPP
#define ZENITH_PRIORITYTHREADPOOL_HPP

//=============
// STD includes
//=============
#include <atomic>
#include <climits>
#include <functional>
#include <mutex>
#include <queue>
#include <string_view>

//=====================
// thread_pool includes
//=====================
#include "PriorityThreadWorker.hpp"
#include "Semaphore.hpp"
#include "ThreadPoolUtility.hpp"
#include "concept.hpp"

namespace thread_pool {

//-----------------------------------------------------------------------------
/// @brief      Thread pool class. thread number is static. detached threads are no longer tracked by the class as they might get terminated before destruction
///
/// @tparam     threadCount  number of threads requested.
///
class PriorityThreadPool {
private:
    PriorityThreadPoolUtility _toolBox;
    std::string_view _name;

    [[nodiscard]] bool safeQueueEmpty() const;

public:
    explicit PriorityThreadPool(uint16_t threadCount, std::string_view name = "zenith_thread");

    ~PriorityThreadPool();

    // we fetch target size, as it is the size the threadpool will converge to. the real size is only temporary as the threadPool cannot modify the thread number outside of its
    // construction.
    int getWorkerNb() const
    {
        std::lock_guard<std::mutex> _(_toolBox.sizeProtection);
        return _toolBox.targetSize;
    }

    // any modifications will only take place AFTER a worker has ended its shift.
    void changeWorkerNb(uint16_t newSize)
    {
        std::lock_guard<std::mutex> _(_toolBox.sizeProtection);
        _toolBox.targetSize = newSize;
    }

    // no verification for overflow. if you ever try to add more than UNSIGNED_16_BITS_MAX, you might want to rethink your life.
    void addWorker()
    {
        std::lock_guard<std::mutex> _(_toolBox.sizeProtection);
        ++_toolBox.targetSize;
    }

    // allow trying to remove worker when there's none. that's not important
    void removeWorker()
    {
        std::lock_guard<std::mutex> _(_toolBox.sizeProtection);
        if (_toolBox.targetSize > 0)
            --_toolBox.targetSize;
    }

    void addJob(std::function<int(void)> priority, sameFunction<helperSameFunction<void(void)>> auto... job)
    {
        std::deque<std::function<void(void)>> jobList {job...};
        std::queue<std::function<void(void)>> realJobList(std::move(jobList));
        {
            const std::lock_guard<std::mutex> _guard(_toolBox.queueProtection);
            _toolBox.jobQueue.emplace_back(priority, std::move(realJobList));
        }
        ++_toolBox.totalJobsPushed;
        _toolBox.jobSemaphore.release(sizeof...(job));
    }

    void addJob(int priority, sameFunction<helperSameFunction<void(void)>> auto... job)
    {
        addJob(
            [priority] {
                return priority;
            },
            job...
        );
    }

    void addJob(sameFunction<helperSameFunction<void(void)>> auto... job) { addJob(0, job...); }

    [[maybe_unused]] void waitUntilJobsDone() const;

    // operator bool() const;
};

inline void waitUntilJobsDone(std::convertible_to<const PriorityThreadPool> auto... args) { (args.waitUntilJobsDone(), ...); }
}

#endif /* ZENITH_THREADPOOL_HPP */
