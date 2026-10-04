//----------------------------------------------------------------
//
// File: MenuConfigure.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/MenuBase.h>
#include <cui/CuiStructs.h>

namespace Cui {

class MenuConfigure : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuConfigure() = default;
    static MenuConfigure& instance();
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
    WindowSize      winSize_ = {10, 37};  // rows, cols

private:
    MenuConfigure();  // Private ctor
    void fillWindow();
    void doFormatRollHistory();
    void doQuickBets();
    void doAutoBets();
    void doTimedRolls();
    void doAnimationSpeed();
    void back();
};

/*-----------------------------------------------------------*//**

@class MenuConfigure

@brief Display choices for game control and configuration

Responsibilities of MenuConfigure

@li Key bindings for the menu
@li Process input keys 
@li Takes action on input keys 
@li Renders the menu on screen
@li Functions to establish defaults and fill values

*/

} // namespace Cui

//----------------------------------------------------------------

