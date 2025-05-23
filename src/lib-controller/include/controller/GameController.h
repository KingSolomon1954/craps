//----------------------------------------------------------------
//
// File: GameController.h
//
//----------------------------------------------------------------

#pragma once

#include <memory>
#include <controller/GameControllerIntfc.h>

namespace Craps {
    class CrapsTable;
}

namespace Ctrl {

class ViewIntfc;  // fwd

class GameController : public GameControllerIntfc
{
public:
    /// @name Lifecycle
    /// @{
    GameController(std::shared_ptr<ViewIntfc> pView);
    Craps::CrapsTable* userSelectsTable();
    void userSelectsPlayers();
    /// @}

    /// @name Modifiers
    /// @{
    void rollDice() override;
    /// @}

    /// @name Observers
    /// @{
    void onAboutCraps() override;
    /// @}

private:
    std::shared_ptr<ViewIntfc> pView_;
};

/*-----------------------------------------------------------*//**

@class GameControllerIntfc

@brief Base interface for view(s) to issue commands to craps game.

more ...

*/

} // namespace Ctrl

//----------------------------------------------------------------
