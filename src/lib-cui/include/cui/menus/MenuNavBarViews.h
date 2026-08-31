//----------------------------------------------------------------
//
// File: MenuNavBarViews.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/MenuBase.h>

namespace Cui {

class MenuNavBarViews : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuNavBarViews();
    /// @}

    /// @name Modifiers
    /// @{
    void draw()             override;
    void handleKey(int ch)  override;
    bool shouldSkip() const override;
    /// @}

    /// @name Observers
    /// @{
    static MenuNavBarViews& instance();
    /// @}
    
private:
    struct Layout
    {
        static constexpr int winStartX = 10;        
        static constexpr int winStartY = 40;
        static constexpr int winHeight = 8;
        static constexpr int winWidth  = 25;
    };

    MenuNavBarViews();  // Private ctor
    void createWindow();
    void fillWindow();
    void allPlayers();
    void nextPlayer();
    void prevPlayer();
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

