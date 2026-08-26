//----------------------------------------------------------------
//
// File: MenuPlayerViews.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/MenuBase.h>

namespace Cui {

class MenuPlayerViews : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuPlayerViewsBet();
    /// @}

    /// @name Modifiers
    /// @{
    void draw()             override;
    void handleKey(int ch)  override;
    bool shouldSkip() const override;
    /// @}

    /// @name Observers
    /// @{
    static MenuPlayerViews& instance();
    /// @}
    
private:
    struct Layout
    {
        static constexpr int winStartX = 10;        
        static constexpr int winStartY = 40;
        static constexpr int winHeight = 8;
        static constexpr int winWidth  = 25;
    };

    MenuPlayerViews();  // Private ctor
    void fillWindow();
    void allPlayers();
    void nextPlayer();
    void prevPlayer();
    void back();
};

/*-----------------------------------------------------------*//**

@class MenuPlayerViews

@brief Display choices for switching Player views

Responsibilities of MenuPlayerViews:

@li Key bindings for the menu

@li Process input keys 

@li Takes action on input keys 

@li Renders the menu on screen

*/

} // namespace Cui

//----------------------------------------------------------------

