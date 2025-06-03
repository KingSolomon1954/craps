//----------------------------------------------------------------
//
// File: ConsoleView.h
//
//----------------------------------------------------------------

#pragma once

#include <boost/asio.hpp>
#include <chrono>
#include <functional>
#include <thread>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace Gen {

class TimerManager {
public:
    using TimerId = int;
    using TimerCallback = std::function<void()>;

    /// @name Lifecycle
    /// @{
    TimerManager();
   ~TimerManager();
    void run();
    void stop();
    /// @}

    /// @name Modifiers
    /// @{
    TimerId createTimer(TimerCallback cb, bool repeat = false);
    TimerId createTimer(TimerCallback cb, std::chrono::milliseconds duration, bool repeat = false);
    void armTimer      (TimerId id, std::chrono::milliseconds duration, bool repeat = false);
    void cancelTimer   (TimerId id);
    void restartTimer  (TimerId id);
    /// @}

    /// @name Observers
    /// @{
    /// @}

private:
    void timerHandler(TimerId id);

    struct TimerEntry
    {
        std::unique_ptr<boost::asio::steady_timer> timer;
        TimerCallback callback;
        std::chrono::milliseconds interval;
        bool repeat = false;
        bool active = false;
    };

    boost::asio::io_context io_;
    std::thread thread_;
    std::unordered_map<TimerId, TimerEntry> timers_;
    std::mutex mutex_;
    TimerId nextId_ = 0;
    bool running_ = false;
};

/*-----------------------------------------------------------*//**

@class TimerManager

@brief Timers, managed with a single thread

@li Multiple timers.
@li Each timer managed by its unique ID.
@li All timers run on a dedicated thread.
@li Repeating timers re-arm themselves.
@li All operations are thread-safe.
@li Can create timers without arming them (createTimer).
@li Arm hem later with a callback (armTimer).
@li Cancel or restart them at any point.
@li Immplementation uses Boost::Asio.

*/

} // namespace Gen

//----------------------------------------------------------------
