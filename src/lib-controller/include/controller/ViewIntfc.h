//----------------------------------------------------------------
//
// File: ViewIntfc.h
//
//----------------------------------------------------------------

#pragma once

namespace Ctrl {

class GameControllerIntfc;  // fwd
    
class ViewIntfc
{
public:
    /// @name Lifecycle
    /// @{
    virtual void setGameController(GameControllerIntfc* pCtrlIntfc) = 0;
    virtual void run() = 0;
    /// @}

    /// @name Modifiers
    /// @{
    virtual void displayMessage() = 0;
    virtual void displayAboutCraps() = 0;
    /// @}

    /// @name Observers
    /// @{
    /// @}
};

/*-----------------------------------------------------------*//**

@class ViewIntfc

@brief Base interface for controller to issue commands to view(s)

more ...

*/

} // namespace Ctrl

//----------------------------------------------------------------
