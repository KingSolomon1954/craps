//----------------------------------------------------------------
//
// File: MenuNavBarViews.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/MenuBase.h>
#include <cui/CuiStructs.h>

namespace Cui {

class MenuNavBarViews : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuNavBarViews() = default;
    /// @}

    /// @name Modifiers
    /// @{
    void draw()                          override;
    bool handleKey(int ch)               override;
    void setLocation(WindowPosition pos) override;
    /// @}

    /// @name Observers
    /// @{
    static MenuNavBarViews& instance();
    LocationRequest getLocationRequest() const override;
    /// @}
    
private:
    WindowPosition  winPos_;
    WindowSize      winSize_ = {8, 25};  // rows, cols

private:
    MenuNavBarViews();  // Private ctor
    void fillWindow();
    void allPlayers();
    void nextPlayer();
    void prevPlayer();
    void popIfActive();
    void back();
};

/*-----------------------------------------------------------*//**

@class MenuNavBarViews

@brief Display choices for switching Player Views.

Comes from the NavBar

Responsibilities of MenuPlayerViews:

@li Key bindings for the menu
@li Process input keys 
@li Takes action on input keys 
@li Draws the menu

*/

} // namespace Cui

//----------------------------------------------------------------

