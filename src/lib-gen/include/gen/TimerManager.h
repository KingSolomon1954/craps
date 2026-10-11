//----------------------------------------------------------------
//
// File: TimerManager.h
//
//----------------------------------------------------------------

#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace Gen {

class TimerManager
{
public:
    using TimerId = int;
    using TimerCallback = std::function<void()>;

    /// @name Lifecycle
    /// @{
    TimerManager();
   ~TimerManager();
    static TimerManager& instance();
    /// @}

    /// @name Modifiers
    /// @{
    TimerId createTimer(TimerCallback cb, bool repeat = false);

    TimerId createTimer(TimerCallback cb,
                        std::chrono::milliseconds duration,
                        bool repeat = false);

    void armTimer(TimerId id,
                  std::chrono::milliseconds duration,
                  bool repeat = false);

    void cancelTimer(TimerId id);
    void restartTimer(TimerId id);
    /// @}

private:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    struct TimerEntry
    {
        TimerCallback callback;
        std::chrono::milliseconds interval{0};
        TimePoint expiresAt{};
        bool repeat = false;
        bool active = false;
        bool configured = false;
    };

    void stop();
    void timerThread();
    bool hasActiveTimer() const;

    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cv_;

    std::unordered_map<TimerId, TimerEntry> timers_;

    TimerId nextId_ = 0;
    bool running_ = false;
};

/*-----------------------------------------------------------*//**

@class TimerManager

@brief Timers, managed with a single thread

@li Multiple timers.
@li Each timer managed by its own unique ID.
@li Repeating timers re-arm themselves.
@li Can create timers immediately armed (createTimer).
@li Can create timers without arming them (createTimer).
@li Arm them later with armTimer().
@li Cancel or restart Timer at any point.
@li All timers run on a dedicated thread.
@li All operations are thread-safe.
*/

} // namespace Gen

//----------------------------------------------------------------
