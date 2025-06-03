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
    void userSelectsTable();
    void userSelectsPlayers();
    /// @}

    /// @name Modifiers
    /// @{
    void dispatchEvent(GameEvent* pBase);
    /// @}

    /// @name Observers
    /// @{
    /// @}

private:
    void onUserInputLine(GameEvent* pBase);
};

/*-----------------------------------------------------------*//**

@class GamerController

@brief Reacts to user input and updates views by processes GameEvents.

more ...

*/

} // namespace Ctrl

//----------------------------------------------------------------
