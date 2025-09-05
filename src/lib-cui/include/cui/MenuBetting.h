//----------------------------------------------------------------
//
// File: MenuBetting.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/Screen.h>

namespace Cui {

class ConsoleView;       // fwd
class ScreenCrapsTable;  // fwd
    
class MenuBetting : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
    MenuBetting(ConsoleView& view)
   ~MenuBetting();
    /// @}

    /// @name Modifiers
    /// @{
    void setOwningScreen(ScreenCrapsTable* pOwning);
    /// @}

    /// @name Observers
    /// @{
    /// @}
    
protected:
    void drawMenu()            override;
    void handleMenuKey(int ch) override;

private:
    ScrrenCrapTable* pOwning_ = nullptr;
};

/*-----------------------------------------------------------*//**

@class MenuBetting

@brief Root menu for ScreeCrapsTable class

Responsibilities of MenuBetting

@li Draws the menu

@li Key bindings for the menu

@li Process input keys 

@li Takes action on input keys 

*/

} // namespace Cui

//----------------------------------------------------------------






class MenuBettingScreen : public Cui::MenuScreen {
public:
    using MenuScreen::MenuScreen;

protected:
    void drawMenu() override {
        mvwprintw(win(), 0, 0, "Betting Menu:");
        mvwprintw(win(), 1, 2, "1) Pass Line Bet");
        mvwprintw(win(), 2, 2, "2) Don't Pass");
        // etc.
    }

    void handleMenuKey(int ch) override {
        switch (ch) {
        case '1': /* handle pass line */ break;
        case '2': /* handle don't pass */ break;
        }
    }
};
