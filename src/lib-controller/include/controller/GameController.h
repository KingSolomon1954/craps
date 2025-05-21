//----------------------------------------------------------------
//
// File: GameControllerIntfc.h
//
//----------------------------------------------------------------

#pragma once

#include <memory>
#include <controller/GameControllerIntfc.h>
#include <craps/CrapsTable.h>

namespace Ctrl {

class ViewIntfc;  // fwd

class GameController : public GameControllerIntfc
{
public:
    /// @name Lifecycle
    /// @{
    GameController(std::shared_ptr<ViewIntfc> pView);
    void chooseTableAndPlayers();
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
    Craps::CrapsTable table_;
};

/*-----------------------------------------------------------*//**

@class GameControllerIntfc

@brief Base interface for view(s) to issue commands to craps game.

more ...

*/

} // namespace Ctrl

//----------------------------------------------------------------
