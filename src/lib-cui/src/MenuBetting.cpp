//----------------------------------------------------------------
//
// File: MenuBetting.cpp
//
//----------------------------------------------------------------

#include <cui/MenuBetting.h>
#include <cui/ConsoleView.h>
#include <cui/ScreenCrapsTable.h>

using namespace Cui;

//----------------------------------------------------------------

MenuBetting::MenuBetting(ConsoleView& view)
    : MenuBase(view, 5, 6)  // TODO Howie need layout
{
}

//----------------------------------------------------------------

MenuBetting::~MenuBetting()
{
}

//----------------------------------------------------------------

void
MenuBetting::setOwningScreen(ScreenCrapsTable* pOwning)
{
    pOwning_ = pOwning;
}

//----------------------------------------------------------------
//
// Override menu base class
//
void
MenuBetting::drawMenu()
{
    // TODO
    mvwprintw(w_, 0, 0, "Betting Menu:");
    mvwprintw(w_, 1, 2, "1) Pass Line Bet");
    mvwprintw(w_, 2, 2, "2) Don't Pass");
    // etc.
}

//----------------------------------------------------------------
//
// Override menu base class
//
void
MenuBetting::handleMenuKey(int ch)
{
    switch (ch)
    {
    case 'p': doPassLine(); break;
    case 'c': doCome();     break;
    }
}

//----------------------------------------------------------------

void
MenuBetting::doPassLine()
{
#if 0
    auto betAmount = amountSubwindow();
    display amount subwindow
#endif
}

//----------------------------------------------------------------

void
MenuBetting::doCome()
{
    
}

//----------------------------------------------------------------

