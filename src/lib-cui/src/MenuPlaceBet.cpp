//----------------------------------------------------------------
//
// File: MenuPlaceBet.cpp
//
//----------------------------------------------------------------

#include <cui/MenuPlaceBet.h>
#include <cstring>
#include <cassert>
#include <cui/ConsoleView.h>

using namespace Cui;

class ConsoleManager;  // fwd
   
//----------------------------------------------------------------

MenuPlaceBet::MenuPlaceBet()
{
    createWindow();
    fillWindow();
}

//----------------------------------------------------------------

MenuPlaceBet*
MenuPlaceBet::instance()
{
    static MenuPlaceBet menu;
    return &menu;
}

//----------------------------------------------------------------

MenuPlaceBet::createWindow()
{
    using L = Layout;
    pWin_ = newwin(L::winHeight, L::winWidth, L::winStartY, L::winStartX);
    
    if (pWin_ == nullptr)
    {
        throw std::runtime_error(
            "Unable to create ncurses MenuPlaceBet window");
    }
}

//----------------------------------------------------------------

MenuPlaceBet::~MenuPlaceBet()
{
    // pWin_ is delwin() in base class
}

//----------------------------------------------------------------
//
// Fills in the window like this, just this once at init time.
// 
// Later, multiple calls to draw() just transfers the already
// filled window.
//
// 0  ┌────────────────────┐
// 1  │ Place Which Number │
// 2  ├────────────────────┤
// 3  │ [4] Place 4        │
// 4  │ [5] Place 5        │
// 5  │ [6] Place 6        │
// 6  │ [8] Place 8        │
// 7  │ [9] Place 9        │
// 8  │ [0] Place 10       │
// 9  │ [esc] Back         │
// 10 └────────────────────┘

void
MenuPlaceBet::fillWindow()
{
    using L = Layout;
    
    // Draw our border.
    box(pWin_, 0, 0);

    // Draw the horizontal separator below the title.
    mvwhline(pWin_, 2, 1, ACS_HLINE, L::winWidth - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, L::winWidth - 1, ACS_RTEE);

    // Static contents. The border occupies row 0/10 and column 0/21.
    mvwaddstr(win, 1, 2, "Place Which Number");

    mvwaddstr(pWin_, 3, 2, "[4] Place 4");
    mvwaddstr(pWin_, 4, 2, "[5] Place 5");
    mvwaddstr(pWin_, 5, 2, "[6] Place 6");
    mvwaddstr(pWin_, 6, 2, "[8] Place 8");
    mvwaddstr(pWin_, 7, 2, "[9] Place 9");
    mvwaddstr(pWin_, 8, 2, "[0] Place 10");
    mvwaddstr(pWin_, 9, 2, "[esc] Back");
}

//----------------------------------------------------------------

void
MenuPivot::draw()
{
    // Reuse already filled window over and over
    CuiUtils::transfer(pWin_);
}    

//----------------------------------------------------------------
//
// Override menu base class
//
void
MenuPivot::handleKey(int ch)
{
    switch(ch)
    {
    case '4': doPivot(4);  break;
    case '5': doPivot(5);  break;
    case '6': doPivot(6);  break;
    case '8': doPivot(8);  break;
    case '9': doPivot(9);  break;
    case '0': doPivot(10); break;
    case 27:  back();      break;
    }
}

//----------------------------------------------------------------

MenuPivot::Results
MenuPivot::getResults() const
{
    Results results;
    results.canceled = isCanceled_;
    results.pivot    = pivot_;
    return results;
}

//----------------------------------------------------------------

void
MenuPivot::clearState()
{
    title_.clear();
    pivot_      = 0;
    betName_    = BetName::Invalid;
    isCanceled_ = false;  // base class
}    

//----------------------------------------------------------------
