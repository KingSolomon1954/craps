//----------------------------------------------------------------
//
// File: MenuBetting.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/MenuBase.h>
#include <craps/EnumBetName.h>

namespace Cui {

class MenuBetting : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuBetting() = default;
    static MenuBetting& instance();
    /// @}

    /// @name Modifiers
    /// @{
    void onResume() override;
    /// @}

    /// @name Observers
    /// @{
    /// @}
    
protected:
    void draw()            override;
    void handleKey(int ch) override;

private:
    struct Layout
    {
        static constexpr int height = 23;
        static constexpr int width  = 25;

        // TODO - get these from geometry manager
        static constexpr int winStartX = 10;        
        static constexpr int winStartY = 40;
    };

    BetName betName_ = BetName::Invalid;

    MenuBetting();  // Private ctor
    void createWindow();
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

