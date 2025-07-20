//----------------------------------------------------------------
//
// File: EventLoop.cpp
//
//----------------------------------------------------------------

#include <controller/EventLoop.h>
#include <controller/Globals.h>
#include <controller/GameController.h>

using namespace Ctrl;

EventLoop::EventLoop()
{
}

//----------------------------------------------------------------

void
EventLoop::enqueue(GameEvent::GameEventPtr ev)
{
    std::lock_guard lock(queueMutex_);
    eventQueue_.push(std::move(ev));
    queueCv_.notify_one();
}

//----------------------------------------------------------------

void
EventLoop::stop()
{
    running_ = false;
    queueCv_.notify_all();
}

//----------------------------------------------------------------

void
EventLoop::run()
{
    running_ = true;
    processEvents();
}

//----------------------------------------------------------------

void
EventLoop::processEvents()
{
    while (running_)
    {
        std::unique_lock lock(queueMutex_);
        queueCv_.wait(lock, [this] { return !eventQueue_.empty() || !running_; });

        while (!eventQueue_.empty())
        {
            GameEvent::GameEventPtr ev = std::move(eventQueue_.front());
            eventQueue_.pop();
            lock.unlock();

            // Controller owns event dispatching
            Gbl::pGameCtrl->dispatchEvent(ev.get());
            
            lock.lock();
        }
    }
}

//----------------------------------------------------------------
