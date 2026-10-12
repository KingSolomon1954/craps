//----------------------------------------------------------------
//
// File: MenuPivot.cpp
//
//----------------------------------------------------------------

#include <cui/menus/MenuPivot.h>

#include <cui/managers/CarrierBet.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/core/CuiUtils.h>
#include <ncurses.h>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

MenuPivot::MenuPivot()
    : MenuBase("MenuPivot")
{
    // Create an initial WINDOW at location 0,0. Gets positioned later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
    drawStaticContent();
}

//----------------------------------------------------------------

MenuPivot&
MenuPivot::instance()
{
    static MenuPivot menu;
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
MenuPivot::configure(BetName betName)
{
    selectTitle  (betName);
    selectPrompts(betName);
}

//----------------------------------------------------------------

void
MenuPivot::selectTitle(BetName name)
{
    switch (name)
    {
    case BetName::Place:
        activeTitle_ = "Place Which Number";
        break;

    case BetName::Buy:
        activeTitle_ = "Buy Which Number  ";
        break;

    case BetName::Lay:
        activeTitle_ = "Lay Which Number  ";
        break;

    }
}

//----------------------------------------------------------------

void
MenuPivot::selectPrompts(BetName name)
{
    switch (name)
    {
    case BetName::Place:
        pNumberPrompts_ = &promptsPlace_;
        break;

    case BetName::Buy:
        pNumberPrompts_ = &promptsBuy_;
        break;

    case BetName::Lay:
        pNumberPrompts_ = &promptsLay_;
        break;
    }
}

//----------------------------------------------------------------
//
// SurfaceManager wants our window size and more.
// This occurs in context of SurfaceManager::pushSurface()
// SurfaceManager informs us shortly of our screen position.
// See setLocation() below. 
//
LocationRequest
MenuPivot::getLocationRequest() const
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
MenuPivot::setLocation(WindowPosition pos)
{
    winPos_ = pos;   // MenuPivot is fixed size, no resizing needed
    repos(winPos_);  // Just need to re-position it
}

//----------------------------------------------------------------

void
MenuPivot::draw()
{
    // Just reuse already filled window over and over
    populate();
    touchwin(pWin_);
    CuiUtils::transfer(pWin_);
}    

//----------------------------------------------------------------
//
// Only done once at init time. These fields are never overwritten
// and We don't call werase().
//
void
MenuPivot::drawStaticContent()
{
    box(pWin_, 0, 0);

    // Horizontal separator below the title
    mvwhline(pWin_, 2, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, winSize_.cols - 1, ACS_RTEE);
    
    mvwaddstr(pWin_, 9, 2, "[. or esc] Back");
}

//----------------------------------------------------------------
//
// Updates dynamic field content.
//
void
MenuPivot::populate()
{
    mvwaddstr(pWin_, 1, 2, activeTitle_.data());

    assert(pNumberPrompts_);
    int row = 3;
    for (const std::string_view s : *pNumberPrompts_)
    {
        mvwaddstr(pWin_, row++, 2, s.data());
    }
}

//----------------------------------------------------------------
//
// Override surface base class
//
bool
MenuPivot::handleKey(int ch)
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
MenuPivot::back()
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
MenuPivot::processSelection(int pivot)
{
    populateCarrier(pivot);
    prepDialogAmount(pivot);
    activateDialogAmount();
}

//----------------------------------------------------------------

void
MenuPivot::populateCarrier(int pivot)
{
    auto cb = CarrierBet::instance();
    cb.clear();
    cb.setBetType(BetName::Place);
    cb.setPivot(pivot);
}

//----------------------------------------------------------------

void
MenuPivot::prepDialogAmount(int pivot)
{
    // TODO
    // DialogBetAmount::setPrompt("Place Bet on %s", pivot);
    // auto amount = getAutoFillAmount(BetType::Pivot, pivot);
    // DialogBetAmount::preFill(amount);
}

//----------------------------------------------------------------

void
MenuPivot::activateDialogAmount()
{
    // TODO
    // SurfaceManager::pushSurface(AmountDialog);
}

//----------------------------------------------------------------
