//----------------------------------------------------------------
//
// File: GameControllerIntfc.h
//
//----------------------------------------------------------------

#pragma once

namespace Ctrl {

class GameControllerIntfc
{
public:
    /// @name Lifecycle
    /// @{
    /// @}

    /// @name Modifiers
    /// @{
    virtual void rollDice() = 0;
    /// @}

    /// @name Observers
    /// @{
    virtual void onAboutCraps() = 0;
    /// @}
};

/*-----------------------------------------------------------*//**

@class GameControllerIntfc

@brief Base interface for view(s) to issue commands to craps game.

more ...

*/

} // namespace Ctrl

//----------------------------------------------------------------
