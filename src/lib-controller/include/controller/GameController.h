//----------------------------------------------------------------
//
// File: GameController.h
//
//----------------------------------------------------------------

#pragma once

#include <memory>
#include <controller/EventLoop.h>
#include <controller/GameEvent.h>

namespace Craps {
    class CrapsTable;
}

namespace Ctrl {

class ViewIntfc;  // fwd
class EventLoop;  // fwd

class GameController
{
public:
    /// @name Lifecycle
    /// @{
    GameController();
    void prepareForShutdown();
//  void userSelectsPlayers();
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
    void onUserInputLine(GameEvent* pBase);
    void onUserInputChar(GameEvent* pBase);
};

/*-----------------------------------------------------------*//**

@class GamerController

@brief Reacts to user input by processing GameEvents.

more ...

*/

} // namespace Ctrl

//----------------------------------------------------------------
