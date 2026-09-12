//----------------------------------------------------------------
//
// File: MenuBetting.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/MenuBase.h>
#include <cui/CuiStructs.h>
#include <craps/EnumBetName.h>

namespace Cui {

class MenuBetting : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuBetting() = default;
    /// @}

    /// @name Modifiers
    /// @{
    void draw()            override;
    bool handleKey(int ch) override;
    void setLocation(WindowPosition pos)       override;
    /// @}

    /// @name Observers
    /// @{
    static MenuBetting& instance();
    LocationRequest getLocationRequest() const override;
    /// @}
    
private:
    WindowPosition  winPos_;
    WindowSize      winSize_ = {23, 25};  // rows, cols
    BetName         betName_ = BetName::Invalid;

private:    
    MenuBetting();  // Private ctor
    void fillWindow();

    // Input handling
    void doBets(BetName betName);
    void doGetAmount();
    void doGetPivot();
    void doOddsBets();
    void doBetFlags();
    void doRemoveBets();
    void doUndoLastBet();
    void doQuickBet();
    void doRollDice();
    void populateCarrier();
    void prepDialogAmount();
    void activateDialogAmount();
    void activateMenuPivot();
    void back();
};

/*-----------------------------------------------------------*//**

@class MenuBetting

@brief Menu for full ScreenCrapsTable

@li Owns a ncurses WINDOW
@li Draws the menu into the window
@li Renders the menu on screen
@li Key bindings for the menu
@li Process input keys 
@li Takes action on input keys 

*/

} // namespace Cui

//----------------------------------------------------------------

