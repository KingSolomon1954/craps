//----------------------------------------------------------------
//
// File: ScreenCrapsTable.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/Screen.h>
#include <cui/ConsoleView.h>
#include <ncurses.h>

namespace Cui {

class ScreenCrapsTable : public Screen
{
public:
    ScreenCrapsTable();
   ~ScreenCrapsTable() override;
    
    void draw()                                override;
    void onActivate(ConsoleView& view)         override;
    void handleInput(const std::string& input) override;

private:
    WINDOW* mainWin_   = nullptr;
    WINDOW* statusWin_ = nullptr;
};

//----------------------------------------------------------------

} // namespace Cui

//----------------------------------------------------------------
