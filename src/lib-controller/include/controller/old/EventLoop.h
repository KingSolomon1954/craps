//----------------------------------------------------------------
//
// File: EventLoop.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/GameEvent.h>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>

namespace Ctrl {

class EventLoop
{
public:
    using Callback = std::function<void(GameEvent*)>;

    /// @name Lifecycle
    /// @{
    explicit EventLoop(const Callback dispatchCb);
   ~EventLoop() = default;
    void stop();
    /// @}

    /// @name Modifiers
    /// @{
    void enqueue(GameEvent::GameEventPtr ev);
    /// @}

    /// @name Observers
    /// @{
    /// @}
    
private:
    // Order doesn't matter
    bool running_ = false;
    std::queue<GameEvent::GameEventPtr> eventQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCv_;
    
    // Order matters
    Callback dispatchEventCb_;
    std::thread thread_;
    
    void run();
    void processEvents();
};

/*-----------------------------------------------------------*//**

@class EventLoop

@brief UI (Console or GUI) enqueues events. The loop processes events.

more ...

*/

} // namespace Ctrl

//----------------------------------------------------------------
