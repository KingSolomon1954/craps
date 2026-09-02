//----------------------------------------------------------------
//
// File: MenuPlaceBet.cpp
//
//----------------------------------------------------------------

#include <cui/menus/MenuPlaceBet.h>

#include <cui/CarrierBet.h>
#include <cui/SurfaceManager.h>
#include <cui/CuiUtils.h>
#include <craps/EnumBetName.h>

using namespace Cui;

//----------------------------------------------------------------

MenuPlaceBet::MenuPlaceBet()
{
    createWindow();
    registerWindow();
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
    newWindow(L::height, L::width,
              L::winStartY, L::winStartX,
              "MenuPlaceBet");
    SurfaceManager::instance().registerForShutdown(this);
}

//----------------------------------------------------------------

void
MenuPlaceBet::registerWindow()
{
    SurfaceManager::instance().registerForShutdown(this);
    surfaceName_ = "MenuPlaceBet";
}

//----------------------------------------------------------------
//
// Fills in the window like this, just this once at init time.
// 
// Later, multiple calls to draw() just transfers the already
// filled window.
//
//    0123456789012345678901
// 0  ┌────────────────────┐
// 1  │ Place Which Number │
// 2  ├────────────────────┤
// 3  │ [4] Place 4        │
// 4  │ [5] Place 5        │
// 5  │ [6] Place 6        │
// 6  │ [8] Place 8        │
// 7  │ [9] Place 9        │
// 8  │ [0] Place 10       │
// 9  │ [. or esc] Back    │
// 10 └────────────────────┘
//
void
MenuPlaceBet::fillWindow()
{
    using L = Layout;
    
    box(pWin_, 0, 0);

    // Horizontal separator below the title
    mvwhline(pWin_, 2, 1, ACS_HLINE, L::width - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, L::width - 1, ACS_RTEE);

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
    case '.':
    case 27:  back();               break;
    }
}

//----------------------------------------------------------------

void
MenuPlaceBet::back()
{
    // Set our own state in base class to reflect cancel.
    // Also informs parent surfaces of the state of operation.
    // In turn, parent menus can decide if they are skipped
    // when unwinding the menu stack.
    //
    setOperationResult(OperationResult::Cancel);  // base class
    SurfaceManager::instance().popSurfaces();
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
    cb.setBetType(BetName::Place);
    cb.setPivot(pivot);
}

//----------------------------------------------------------------

void
MenuPlaceBet::prepDialogAmount(int pivot)
{
    // TODO
    // DialogBetAmount::setPrompt("Place Bet on %s", pivot);
    // auto amount = getAutoFillAmount(BetType::PlaceBet, pivot);
    // DialogBetAmount::preFill(amount);
}

//----------------------------------------------------------------

void
MenuPlaceBet::activateDialogAmount()
{
    // TODO
    // SurfaceManager::pushSurface(AmountDialog);
}

//----------------------------------------------------------------
