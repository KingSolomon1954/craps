//----------------------------------------------------------------
//
// File: MenuPlaceBet.cpp
//
//----------------------------------------------------------------

#include <cui/MenuPlaceBet.h>
#include <cui/CarrierBet.h>
#include <craps/EnumBetName.h>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

MenuPlaceBet::MenuPlaceBet()
{
    createWindow();
    fillWindow();
}

//----------------------------------------------------------------

MenuPlaceBet&
MenuPlaceBet::instance()
{
    static MenuPlaceBet menu;
    return menu;
}

//----------------------------------------------------------------

void
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
//
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
    mvwaddstr(pWin_, 1, 2, "Place Which Number");

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
MenuPlaceBet::draw()
{
    // Just reuse already filled window over and over
    CuiUtils::transfer(pWin_);
}    

//----------------------------------------------------------------
//
// Override surface base class
//
void
MenuPlaceBet::handleKey(int ch)
{
    switch(ch)
    {
    case '4': processSelection(4);  break;
    case '5': processSelection(5);  break;
    case '6': processSelection(6);  break;
    case '8': processSelection(8);  break;
    case '9': processSelection(9);  break;
    case '0': processSelection(10); break;
    case 27:  quit();               break;
    }
}

//----------------------------------------------------------------

void
MenuPlaceBet::processSelection(int pivot)
{
    populateCarrier(pivot);
    prepDialogAmount(pivot);
    activateDialogAmount();
}

//----------------------------------------------------------------

void
MenuPlaceBet::populateCarrier(int pivot)
{
    auto cb = CarrierBet::instance();
    cb.clear();
    cb.setBetType(BetName::PlaceBet);
    cb.setPivot(pivot);
}

//----------------------------------------------------------------

void prepDialogAmount(int pivot)
{
    DialogBetAmount::setPrompt("Place Bet on %s", pivot);
    auto amount = getAutoFillAmount(BetType::PlaceBet, pivot);
    DialogBetAmount::preFill(amount);
}

//----------------------------------------------------------------

void
MenuPlaceBet::activateDialogAmount()
{
    ConsoleManager::pushSurface(AmountDialog);
}

//----------------------------------------------------------------

void
MenuPlaceBet::quit()
{
    // Set our own state in base class to reflect cancel.
    // Also informs parent surfaces of the state of operation.
    // In turn, parent menus can decide if they are skipped
    // when unwinding the menu stack.
    //
    setOperationResult(OperationResult::cancel);
    
    ConsoleManager::popSurfaces();
}

//----------------------------------------------------------------
