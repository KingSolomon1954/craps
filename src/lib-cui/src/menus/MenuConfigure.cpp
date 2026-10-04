//----------------------------------------------------------------
//
// File: MenuConfigure.cpp
//
//----------------------------------------------------------------

#include <cui/menus/MenuConfigure.h>
#include <cui/CuiUtils.h>
#include <cui/SurfaceManager.h>

using namespace Cui;

//----------------------------------------------------------------

MenuConfigure::MenuConfigure()
    : MenuBase("MenuConfigure")
{
    // Create an initial WINDOW at location 0,0. Gets positioned later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
    fillWindow();
}

//----------------------------------------------------------------

MenuConfigure&
MenuConfigure::instance()
{
    static MenuConfigure menu;
    return menu;
}

//----------------------------------------------------------------
//
// Fills in the window like this, just this once at init time.
// 
// Later, multiple calls to draw() just transfers the already
// filled window.
//
//    0123456789 123456789 123456
// 0  ┌─────────────────────────┐
// 1  │ Configure               │
// 2  ├─────────────────────────┤
// 3  │ [f] Format Roll History │
// 4  │ [q] Quick Bets          │
// 5  │ [a] Auto Bets           │
// 6  │ [t] Timed Rolls         │
// 7  │ [s] Animation Speed     │
// 8  │ [. or esc] Back         │
// 9  └─────────────────────────┘
//
void
MenuConfigure::fillWindow()
{
    box(pWin_, 0, 0);

    // Horizontal separator below the title
    mvwhline(pWin_, 2, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, winSize_.cols - 1, ACS_RTEE);

    mvwaddstr(pWin_, 1, 2, "Configure");
    mvwaddstr(pWin_, 3, 2, "[c] Format Roll History");
    mvwaddstr(pWin_, 4, 2, "[q] Quick Bets");
    mvwaddstr(pWin_, 5, 2, "[a] Auto Bets");
    mvwaddstr(pWin_, 6, 2, "[t] Timed Rolls");
    mvwaddstr(pWin_, 7, 2, "[s] Animation Speed");
    mvwaddstr(pWin_, 8, 2, "[. or esc] Back");
}

//----------------------------------------------------------------
//
// SurfaceManager wants our window size and more.
// This occurs in context of SurfaceManager::pushSurface()
// SurfaceManager informs us shortly of our screen position.
// See setLocation() below. 
//
LocationRequest
MenuConfigure::getLocationRequest() const
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
MenuConfigure::setLocation(WindowPosition pos)
{
    winPos_ = pos;   // MenuConfigure is fixed size, no resizing needed
    repos(winPos_);  // Just need to re-position it
}

//----------------------------------------------------------------

void
MenuConfigure::draw()
{
    // Just reuse already filled window over and over
    CuiUtils::transfer(pWin_);
}    

//----------------------------------------------------------------
//
// Override surface base class
//
bool
MenuConfigure::handleKey(int ch)
{
    bool handled = true;
    switch(ch)
    {
    case 'f': doFormatRollHistory(); break;
    case 'q': doQuickBets();         break;
    case 'a': doAutoBets();          break;
    case 't': doTimedRolls();        break;
    case 's': doAnimationSpeed();    break;
    case '.':
    case 27 : back();                break;
    default : handled = false;       break;
    }
    return handled;
}

//----------------------------------------------------------------

void
MenuConfigure::doFormatRollHistory()
{
    // TODO
}

//----------------------------------------------------------------

void
MenuConfigure::doQuickBets()
{
    // TODO
}

//----------------------------------------------------------------

void
MenuConfigure::doAutoBets()
{
    // TODO
}

//----------------------------------------------------------------

void
MenuConfigure::doTimedRolls()
{
    // TODO
}

//----------------------------------------------------------------

void
MenuConfigure::doAnimationSpeed()
{
    // TODO
}

//----------------------------------------------------------------

void
MenuConfigure::back()
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
