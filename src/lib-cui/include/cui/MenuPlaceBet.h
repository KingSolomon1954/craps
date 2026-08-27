//----------------------------------------------------------------
//
// File: MenuPlaceBet.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/MenuBase.h>

namespace Cui {

class MenuPlaceBet : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuPlaceBet();
    /// @}

    /// @name Modifiers
    /// @{
    void draw()             override;
    void handleKey(int ch)  override;
    // bool shouldSkip() const override;  base class default is good
    /// @}

    /// @name Observers
    /// @{
    static MenuPlaceBet& instance();
    /// @}
    
private:
    struct Layout
    {
        static constexpr int winStartX = 10;        
        static constexpr int winStartY = 40;
        static constexpr int winHeight = 11;
        static constexpr int winWidth  = 22;
    };

    MenuPlaceBet();  // Private ctor
    void fillWindow();
    void processSelection(int num);
    void populateCarrier();
    void prepDialogAmount();
    void activateDialogAmount();
    void quit();
};

/*-----------------------------------------------------------*//**

@class MenuPlaceBet

@brief Display choices for Place Bets

Responsibilities of MenuPlaceBet

@li Key bindings for the menu

@li Process input keys 

@li Takes action on input keys 

@li Renders the menu on screen

@li Functions to establish defaults and fill values

*/

} // namespace Cui

//----------------------------------------------------------------

