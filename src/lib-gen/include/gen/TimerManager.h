// TimerManager.h
#pragma once

#include <boost/asio.hpp>
#include <chrono>
#include <functional>
#include <thread>
#include <unordered_map>
#include <memory>
#include <mutex>

class TimerManager {
public:
    using TimerId = int;
    using TimerCallback = std::function<void()>;

    TimerManager();
    ~TimerManager();

    void run();
    void stop();

    TimerId addTimer(std::chrono::milliseconds duration, TimerCallback cb, bool repeat = false);
    TimerId createTimer(std::chrono::milliseconds duration, bool repeat = false);
    void armTimer(TimerId id, TimerCallback cb);
    void cancelTimer(TimerId id);
    void restartTimer(TimerId id);

private:
    void timerHandler(TimerId id);

    struct TimerEntry {
        std::unique_ptr<boost::asio::steady_timer> timer;
        TimerCallback callback;
        std::chrono::milliseconds interval;
        bool repeat;
        bool active = false;
    };

    boost::asio::io_context io_;
    std::thread thread_;
    std::unordered_map<TimerId, TimerEntry> timers_;
    std::mutex mutex_;
    TimerId nextId_ = 0;
    bool running_ = false;
};


