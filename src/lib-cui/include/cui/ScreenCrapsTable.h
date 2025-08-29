//----------------------------------------------------------------
//
// File: ScreenCrapsTable.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/Screen.h>
#include <ncurses.h>

namespace Cui {

class ScreenCrapsTable : public Screen
{
public:
    ScreenCrapsTable(ConsoleView& view);
   ~ScreenCrapsTable() override;
    
    void draw()            override;
    void onAttach()        override;
    void handleKey(int ch) override;

private:
    struct Windows
    {
        WINDOW *header,  *animation,
               *history, *house,
               *table,   *player,
               *message, *command;
    };

    enum class Menus
    {
        Betting,
        Stats
    };

    Windows w_;
    std::string lineBuffer_;
    Menus activeMenu_;

    Windows createSubwindows();
    void handleLineInput(int ch);
    void handleMenuInput(int ch);
    void menuInputBetting(int ch);
    void menuInputStats(int ch);
    void setMenuInputMode();
    void setLineInputMode();
    void redrawInputPrompt();
    void processLineBuffer();
};

//----------------------------------------------------------------

} // namespace Cui

//----------------------------------------------------------------
