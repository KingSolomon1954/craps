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
    : MenuBase("MenuPlaceBet")
{
    // Create an initial WINDOW at location 0,0. Gets positioned later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
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
    box(pWin_, 0, 0);

    // Horizontal separator below the title
    mvwhline(pWin_, 2, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, winSize_.cols - 1, ACS_RTEE);

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
//
// SurfaceManager wants our window size and more.
// This occurs in context of SurfaceManager::pushSurface()
// SurfaceManager informs us shortly of our screen position.
// See setLocation() below. 
//
LocationRequest
MenuPlaceBet::getLocationRequest() const
{
    LocationRequest req;
    req.kind      = LocationKind::Menu;
    req.size.rows = winSize_.rows;
    req.size.cols = winSize_.cols;
    req.direction = Direction::Right;
    return req;
}

//----------------------------------------------------------------
//
// SurfaceManager tells us our location.
// This occurs in context of SurfaceManager::pushSurface().
// We now have enough information to create/resize our
// ncurses WINDOW. Following this, SurfaceManager will
// call draw() on us.
//
void
MenuPlaceBet::setLocation(WindowPosition pos)
{
    winPos_ = pos;   // MenuPlaceBet is fixed size, no resizing needed
    repos(winPos_);  // Just need to re-position it
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
bool
MenuPlaceBet::handleKey(int ch)
{
    bool handled = true;
    switch(ch)
    {
    case '4': processSelection(4);  break;
    case '5': processSelection(5);  break;
    case '6': processSelection(6);  break;
    case '8': processSelection(8);  break;
    case '9': processSelection(9);  break;
    case '0': processSelection(10); break;
    case '.':
    case 27 : back();               break;
    default : handled = false;      break;
    }
    return handled;
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
