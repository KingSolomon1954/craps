//----------------------------------------------------------------
//
// File: MenuView.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/MenuBase.h>

namespace Cui {

class MenuView : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuViewBet();
    /// @}

    /// @name Modifiers
    /// @{
    void draw()             override;
    void handleKey(int ch)  override;
    bool shouldSkip() const override;
    /// @}

    /// @name Observers
    /// @{
    static MenuView& instance();
    /// @}
    
private:
    struct Layout
    {
        static constexpr int winStartX = 10;        
        static constexpr int winStartY = 40;
        static constexpr int winHeight = 8;
        static constexpr int winWidth  = 25;
    };

    MenuView();  // Private ctor
    void fillWindow();
    void allPlayers();
    void nextPlayer();
    void prevPlayer();
    void back();
};

/*-----------------------------------------------------------*//**

@class MenuView

@brief Display choices for switching Player views

Responsibilities of MenuView:

@li Key bindings for the menu

@li Process input keys 

@li Takes action on input keys 

@li Renders the menu on screen

*/

} // namespace Cui

//----------------------------------------------------------------

