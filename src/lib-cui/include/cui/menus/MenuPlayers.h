//----------------------------------------------------------------
//
// File: MenuPlayers.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/MenuBase.h>
#include <cui/core/CuiStructs.h>

namespace Cui {

class MenuPlayers : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuPlayers() = default;
    static MenuPlayers& instance();
    /// @}

    /// @name Modifiers
    /// @{
    void draw()                          override;
    bool handleKey(int ch)               override;
    void setLocation(WindowPosition pos) override;
    /// @}

    /// @name Observers
    /// @{
    LocationRequest getLocationRequest() const override;
    /// @}
    
private:
    WindowPosition   winPos_{};
    const WindowSize winSize_ = {13, 26};  // rows, cols

private:
    MenuPlayers();  // Private ctor
    void fillWindow();
    void doListPlayers();
    void doSwitchToPlayer();
    void doRenamePlayer();
    void doCreatePlayer();
    void doDeletePlayer();
    void doListBuddies();
    void doBuddyJoinsTable();
    void doBuddyLeavesTable();
    void back();
};

/*-----------------------------------------------------------*//**

@class MenuPlayers

@brief Display choices for game control and configuration

Responsibilities of MenuPlayers

@li Key bindings for the menu
@li Process input keys 
@li Takes action on input keys 
@li Renders the menu on screen
@li Functions to establish defaults and fill values

*/

} // namespace Cui

//----------------------------------------------------------------

