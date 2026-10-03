//----------------------------------------------------------------
//
// File: MenuControl.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/MenuBase.h>
#include <cui/CuiStructs.h>

namespace Cui {

class MenuControl : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuControl() = default;
    static MenuControl& instance();
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
    WindowPosition  winPos_;
    WindowSize      winSize_ = {14, 25};  // rows, cols

private:
    MenuControl();  // Private ctor
    void fillWindow();
    void doConfigure();
    void doPause();
    void doResume();
    void doBuddyJoins();
    void doBuddyLeaves();
    void doRenamePlayer();
    void doSwitchPlayer();
    void doCreatePlayer();
    void doChangeTable();
    void back();
};

/*-----------------------------------------------------------*//**

@class MenuControl

@brief Display choices for game control and configuration

Responsibilities of MenuControl

@li Key bindings for the menu
@li Process input keys 
@li Takes action on input keys 
@li Renders the menu on screen
@li Functions to establish defaults and fill values

*/

} // namespace Cui

//----------------------------------------------------------------

