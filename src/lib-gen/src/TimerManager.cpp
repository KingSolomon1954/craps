//----------------------------------------------------------------
//
// File: TimerManager.cpp
//
//----------------------------------------------------------------

#include <gen/TimerManager.h>

using namespace Gen;

//----------------------------------------------------------------

TimerManager::TimerManager()
    : running_(true)
    , thread_(&TimerManager::timerThread, this)
{
}

//----------------------------------------------------------------

TimerManager&
TimerManager::instance()
{
    static TimerManager tm;
    return tm;
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
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!running_)
        {
            return;
        }

        running_ = false;
    }

    cv_.notify_one();
}

//----------------------------------------------------------------

TimerManager::TimerId
TimerManager::createTimer(TimerCallback cb, bool repeat)
{
    std::lock_guard<std::mutex> lock(mutex_);

    TimerId id = nextId_++;

    timers_.emplace(
        id,
        TimerEntry
        {
            std::move(cb),
            std::chrono::milliseconds{0},
            TimePoint{},
            repeat,
            false,
            false
        });

    return id;
}

//----------------------------------------------------------------

TimerManager::TimerId
TimerManager::createTimer(
    TimerCallback cb,
    std::chrono::milliseconds duration,
    bool repeat)
{
    TimerId id = createTimer(std::move(cb), repeat);

    armTimer(id, duration, repeat);

    return id;
}

//----------------------------------------------------------------

void
TimerManager::armTimer(
    TimerId id,
    std::chrono::milliseconds duration,
    bool repeat)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = timers_.find(id);
        if (it == timers_.end())
        {
            return;
        }

        auto& entry = it->second;

        entry.interval = duration;
        entry.repeat = repeat;
        entry.expiresAt = Clock::now() + duration;
        entry.active = true;
        entry.configured = true;
    }

    // The earliest timer may have changed.
    cv_.notify_one();
}

//----------------------------------------------------------------

void
TimerManager::cancelTimer(TimerId id)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = timers_.find(id);
        if (it == timers_.end())
        {
            return;
        }

        // Preserve interval/repeat/configured.
        it->second.active = false;
    }

    cv_.notify_one();
}

//----------------------------------------------------------------

void
TimerManager::restartTimer(TimerId id)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = timers_.find(id);
        if (it == timers_.end())
        {
            return;
        }

        auto& entry = it->second;

        // Never armed/configured, so there is nothing to restart.
        if (!entry.configured || !entry.callback)
        {
            return;
        }

        entry.expiresAt = Clock::now() + entry.interval;
        entry.active = true;
    }

    cv_.notify_one();
}

//----------------------------------------------------------------

bool
TimerManager::hasActiveTimer() const
{
    for (const auto& [id, entry] : timers_)
    {
        if (entry.active)
        {
            return true;
        }
    }

    return false;
}

//----------------------------------------------------------------

void
TimerManager::timerThread()
{
    std::unique_lock<std::mutex> lock(mutex_);

    while (running_)
    {
        // Find the earliest active timer.
        auto nextTimer = timers_.end();

        for (auto it = timers_.begin(); it != timers_.end(); ++it)
        {
            if (!it->second.active)
            {
                continue;
            }

            if (nextTimer == timers_.end() ||
                it->second.expiresAt < nextTimer->second.expiresAt)
            {
                nextTimer = it;
            }
        }

        // No active timers. Wait until something changes.
        if (nextTimer == timers_.end())
        {
            cv_.wait(lock, [this]
            {
                return !running_ || hasActiveTimer();
            });

            continue;
        }

        const auto expiresAt = nextTimer->second.expiresAt;

        // Wait until this timer expires, or until another
        // operation changes the timer set.
        if (cv_.wait_until(lock, expiresAt) ==
            std::cv_status::timeout)
        {
            if (!running_)
            {
                break;
            }

            const auto now = Clock::now();

            // Re-check because the timer may have been
            // cancelled or restarted while we were waiting.
            if (!nextTimer->second.active ||
                nextTimer->second.expiresAt > now)
            {
                continue;
            }

            auto& entry = nextTimer->second;

            TimerCallback callback = entry.callback;

            if (entry.repeat)
            {
                entry.expiresAt = Clock::now() + entry.interval;
            }
            else
            {
                entry.active = false;
            }

            // Never invoke client code while holding mutex_.
            lock.unlock();

            if (callback)
            {
                callback();
            }

            lock.lock();
        }
    }
}

//----------------------------------------------------------------
