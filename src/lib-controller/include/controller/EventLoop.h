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
    /// @name Lifecycle
    /// @{
    EventLoop();
    void run();
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
    void processEvents();

    bool running_ = false;
    std::queue<GameEvent::GameEventPtr> eventQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCv_;
};

/*-----------------------------------------------------------*//**

@class ConsoleView

@brief Console interaction with the user.

more ...

*/

} // namespace Ctrl

//----------------------------------------------------------------
