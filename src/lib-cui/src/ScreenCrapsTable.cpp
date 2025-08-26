//----------------------------------------------------------------
//
// File: ScreenCrapsTable.cpp
//
//----------------------------------------------------------------

#include <cui/ScreenCrapsTable.h>

using namespace Cui;

//----------------------------------------------------------------

ScreenCrapsTable::ScreenCrapsTable()
{
    mainWin_ = newwin(1, 150, 50, 0);
    // statusWin_ = newwin(1, cols, rows-3, 0);
}

//----------------------------------------------------------------

ScreenCrapsTable::~ScreenCrapsTable()
{
    if (mainWin_) delwin(mainWin_);
}

//----------------------------------------------------------------

void
ScreenCrapsTable::draw()
{
    werase(mainWin_);
    box(mainWin_, 0, 0);
    mvwprintw(mainWin_, 0, 2, "[ Craps Table ]");
    wrefresh(mainWin_);

    werase(statusWin_);
    mvwprintw(statusWin_, 0, 0, "Status: at the table");
    wrefresh(statusWin_);
}

//----------------------------------------------------------------

void
ScreenCrapsTable::onActivate(ConsoleView& view)
{
    view.setInputMode(ConsoleView::InputMode::Line);
}

//----------------------------------------------------------------

void
ScreenCrapsTable::handleInput(const std::string& input)
{
    char choice = input[0];
    if (choice == 'a')
    {
        // handle selection
    }

}

//----------------------------------------------------------------
