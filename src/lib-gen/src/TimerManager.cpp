// TimerManager.cpp
#include "TimerManager.h"

TimerManager::TimerManager() : running_(true) {
    thread_ = std::thread([this]() { io_.run(); });
}

TimerManager::~TimerManager() {
    stop();
    if (thread_.joinable()) {
        thread_.join();
    }
}

void TimerManager::run() {
    if (!running_) {
        running_ = true;
        thread_ = std::thread([this]() { io_.run(); });
    }
}

void TimerManager::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
    io_.stop();
}

TimerManager::TimerId TimerManager::addTimer(std::chrono::milliseconds duration, TimerCallback cb, bool repeat) {
    TimerId id = nextId_++;
    std::lock_guard<std::mutex> lock(mutex_);

    TimerEntry entry{
        std::make_unique<boost::asio::steady_timer>(io_, duration),
        cb,
        duration,
        repeat,
        true
    };

    auto handler = [this, id](const boost::system::error_code& ec) {
        if (!ec) {
            timerHandler(id);
        }
    };

    entry.timer->async_wait(handler);
    timers_.emplace(id, std::move(entry));
    return id;
}

TimerManager::TimerId TimerManager::createTimer(std::chrono::milliseconds duration, bool repeat) {
    TimerId id = nextId_++;
    std::lock_guard<std::mutex> lock(mutex_);

    TimerEntry entry{
        std::make_unique<boost::asio::steady_timer>(io_),
        nullptr,
        duration,
        repeat,
        false
    };

    timers_.emplace(id, std::move(entry));
    return id;
}

void TimerManager::armTimer(TimerId id, TimerCallback cb) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = timers_.find(id);
    if (it == timers_.end()) return;

    auto& entry = it->second;
    entry.callback = cb;
    entry.active = true;
    entry.timer->expires_after(entry.interval);
    entry.timer->async_wait([this, id](const boost::system::error_code& ec) {
        if (!ec) timerHandler(id);
    });
}

void TimerManager::cancelTimer(TimerId id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = timers_.find(id);
    if (it != timers_.end()) {
        it->second.timer->cancel();
        it->second.active = false;
    }
}

void TimerManager::restartTimer(TimerId id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = timers_.find(id);
    if (it == timers_.end()) return;

    auto& entry = it->second;
    if (!entry.active || !entry.callback) return;

    entry.timer->expires_after(entry.interval);
    entry.timer->async_wait([this, id](const boost::system::error_code& ec) {
        if (!ec) timerHandler(id);
    });
}

void TimerManager::timerHandler(TimerId id) {
    TimerCallback cb;
    bool repeat = false;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = timers_.find(id);
        if (it == timers_.end() || !it->second.active) return;

        cb = it->second.callback;
        repeat = it->second.repeat;

        if (repeat) {
            it->second.timer->expires_after(it->second.interval);
            it->second.timer->async_wait([this, id](const boost::system::error_code& ec) {
                if (!ec) timerHandler(id);
            });
        } else {


            it->second.active = false;
        }
    }

    if (cb) cb();
}
