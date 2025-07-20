//----------------------------------------------------------------
//
// File: TimerManager.h
//
//----------------------------------------------------------------

#include <gen/TimerManager.h>

using namespace Gen;

//----------------------------------------------------------------

TimerManager::TimerManager()
    : io_()
    , work_(boost::asio::make_work_guard(io_))
    , running_(true)
    , thread_(std::thread([this]() {io_.run(); }))
{
}

//----------------------------------------------------------------

TimerManager::~TimerManager()
{
    stop();
    if (thread_.joinable())
    {
        thread_.join();
    }
}

//----------------------------------------------------------------

void
TimerManager::stop()
{
    std::lock_guard<std::mutex> lock(mutex_);
    running_ = false;
    work_.reset();  // allow run() to exit
    io_.stop();
}

//----------------------------------------------------------------

TimerManager::TimerId
TimerManager::createTimer(TimerCallback cb, bool repeat)
{
    TimerId id = nextId_++;
    std::lock_guard<std::mutex> lock(mutex_);
    TimerEntry entry
    {
        std::make_unique<boost::asio::steady_timer>(io_),
        cb,
        std::chrono::milliseconds{0},
        repeat,
        false
    };
    timers_.emplace(id, std::move(entry));
    return id;
}

//----------------------------------------------------------------

TimerManager::TimerId
TimerManager::createTimer(
    TimerCallback cb,
    std::chrono::milliseconds duration,
    bool repeat)
{
    TimerId id = createTimer(cb, repeat);
    armTimer(id, duration, repeat);
    return id;
}

//----------------------------------------------------------------

void
TimerManager::armTimer(TimerId id,
                       std::chrono::milliseconds duration,
                       bool repeat)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = timers_.find(id);
    if (it == timers_.end()) return;

    auto& entry = it->second;
    entry.interval = duration;
    entry.repeat = repeat;
    entry.active = true;

    entry.timer->expires_after(duration);
    entry.timer->async_wait(
        [this, id](const boost::system::error_code& ec)
        {
            if (!ec) timerHandler(id);
        }
    );
}

//----------------------------------------------------------------

void
TimerManager::cancelTimer(TimerId id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = timers_.find(id);
    if (it != timers_.end())
    {
        it->second.timer->cancel();
        it->second.active = false;
    }
}

//----------------------------------------------------------------

void
TimerManager::restartTimer(TimerId id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = timers_.find(id);
    if (it == timers_.end()) return;

    auto& entry = it->second;
    if (!entry.active || !entry.callback) return;

    entry.timer->expires_after(entry.interval);
    entry.timer->async_wait(
        [this, id](const boost::system::error_code& ec)
        {
            if (!ec) timerHandler(id);
        }
    );
}

//----------------------------------------------------------------

void
TimerManager::timerHandler(TimerId id)
{
    TimerCallback cb;
    bool repeat = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = timers_.find(id);
        if (it == timers_.end() || !it->second.active) return;

        cb = it->second.callback;
        repeat = it->second.repeat;

        if (repeat)
        {
            it->second.timer->expires_after(it->second.interval);
            it->second.timer->async_wait(
                [this, id](const boost::system::error_code& ec)
                {
                    if (!ec) timerHandler(id);
                }
            );
        }
        else
        {
            it->second.active = false;
        }
    }
    if (cb) cb();
}

//----------------------------------------------------------------
