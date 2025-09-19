//----------------------------------------------------------------
//
// File: GameController.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/EventLoop.h>
#include <controller/GameEvent.h>

namespace Ctrl {

class EventLoop;  // fwd

class GameController
{
public:
    /// @name Lifecycle
    /// @{
    GameController();
    void prepareForShutdown();
    /// @}

    /// @name Modifiers
    /// @{
    void enqueue(GameEvent::GameEventPtr ev);
    /// @}

    /// @name Observers
    /// @{
    /// @}

private:
    EventLoop eventLoop_;
    
    void dispatchEvent(GameEvent* pBase);
};

/*-----------------------------------------------------------*//**

@class GamerController

@brief Reacts to user input by processing GameEvents.

Has these responsibilities:

@li The owner of event ordering.
@li The only source of correlation IDs
@lib The router between UI-facing code and model-facing code.

*/

} // namespace Ctrl

//----------------------------------------------------------------
