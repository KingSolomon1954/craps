//----------------------------------------------------------------
//
// File: MenuNavBarViews.cpp
//
//----------------------------------------------------------------

#include <cui/menus/MenuNavBarViews.h>
#include <cui/menus/MenuBetting.h>
#include <cui/panels/WindowPlayerArea.h>
#include <cui/SurfaceManager.h>
#include <cui/CuiUtils.h>
#include <gen/Logger.h>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

MenuNavBarViews::MenuNavBarViews()
    : MenuBase("MenuNavBarViews")
{
    // Create an initial WINDOW at location 0,0. Gets positioned later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
    fillWindow();    
}

//----------------------------------------------------------------

MenuNavBarViews&
MenuNavBarViews::instance()
{
    static MenuNavBarViews menu;
    return menu;
}

//----------------------------------------------------------------
//
// Fills in the window like this, just this once at init time.
// 
// Later, multiple calls to draw() just transfers the already
// filled window.
//
// 0  ┌───────────────────────┐
// 1  │ View Menu             │
// 2  ├───────────────────────┤
// 3  │ [A] All Players       │
// 4  │ [N] One Player (next) │
// 5  │ [P] One Player (prev) │
// 6  │ [. or esc] Back       │
// 7  └───────────────────────┘
//
void
MenuNavBarViews::fillWindow()
{
    // Draw our border.
    box(pWin_, 0, 0);

    // Draw the horizontal separator below the title.
    mvwhline(pWin_, 2, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, winSize_.cols - 1, ACS_RTEE);

    // Static contents. The border occupies row 0/10 and column 0/21.
    mvwaddstr(pWin_, 1, 2, "View Menu");

    mvwaddstr(pWin_, 3, 2, "[A] All Players");
    mvwaddstr(pWin_, 4, 2, "[N] One Player (next)");
    mvwaddstr(pWin_, 5, 2, "[P] One PLayer (prev)");
    mvwaddstr(pWin_, 6, 2, "[. or esc] Back");
}

//----------------------------------------------------------------
//
// SurfaceManager wants our window size and more.
// This occurs in context of SurfaceManager::pushSurface()
// SurfaceManager informs us shortly of our screen position.
// See setLocation() below. 
//
LocationRequest
MenuNavBarViews::getLocationRequest() const
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
MenuNavBarViews::setLocation(WindowPosition pos)
{
    winPos_ = pos;   // MenuNavBarViews is fixed size
    repos(winPos_);  // Just need position
}

//----------------------------------------------------------------

void
MenuNavBarViews::draw()
{
    LOG_TRACE("MenuNavBarViews::draw() ");
    CuiUtils::transfer(pWin_);  // Reuse already filled window over and over
}    

//----------------------------------------------------------------
//
// Override surface base class
//
bool
MenuNavBarViews::handleKey(int ch)
{
    bool handled = true;
    switch(ch)
    {
    case 'A': allPlayers();    break;
    case 'N': nextPlayer();    break;
    case 'P': prevPlayer();    break;
    case '.':
    case 27 : back();          break;
    default : handled = false; break;
    }
    return handled;
}

//----------------------------------------------------------------
//
// Three conditions to handle below:
//
// 1. ScreenCrapsTable is the active window
//    call ScreenCrapsTable nextPlayer, prevPlayer, or allPlayers
//    trigger redraw
//
// 2. MenuBetting is the active window
//    call ScreenCrapsTable nextPlayer, prevPlayer, or allPlayers
//    popSurface() MenuBetting disappears, ScreenCrapsTable is redrawn 
//
// 3. MenuNavBarViews is the active window
//    call ScreenCrapsTable nextPlayer, prevPlayer, or allPlayers
//    popSurface() MenuNavBarViews disappears, ScreenCrapsTable is redrawn 


//----------------------------------------------------------------

void
MenuNavBarViews::popIfActive()
{
    if (SurfaceManager::instance().isActiveSurface(this))
    {
        SurfaceManager::instance().popSurface();
    }
    else
    {
        // Else ScreenCrapsTable is active, repaint
        SurfaceManager::instance().draw();
    }
}

//----------------------------------------------------------------

void
MenuNavBarViews::allPlayers()
{
    WindowPlayerArea::instance().allPlayers();
    popIfActive();
}

//----------------------------------------------------------------

void
MenuNavBarViews::nextPlayer()
{
    WindowPlayerArea::instance().nextPlayer();
    popIfActive();
}

//----------------------------------------------------------------

void
MenuNavBarViews::prevPlayer()
{
    WindowPlayerArea::instance().prevPlayer();
    popIfActive();
}

//----------------------------------------------------------------

void
MenuNavBarViews::back()
{
    if (SurfaceManager::instance().isActiveSurface(this))
    {
        SurfaceManager::instance().popSurface();  // Become invisible
    }

    // Else ignore, nothing to do. ScreenCrapsTable is visible
}

//----------------------------------------------------------------
