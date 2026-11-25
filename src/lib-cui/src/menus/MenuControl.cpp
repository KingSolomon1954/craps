//----------------------------------------------------------------
//
// File: MenuControl.cpp
//
//----------------------------------------------------------------

#include <cui/menus/MenuControl.h>
#include <cui/menus/MenuConfigure.h>
#include <cui/menus/MenuPlayers.h>
#include <cui/menus/MenuTable.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/core/CuiUtils.h>
#include <gen/Logger.h>

using namespace Cui;

//----------------------------------------------------------------

MenuControl::MenuControl()
    : MenuBase("MenuControl")
{
    // Create an initial WINDOW at location 0,0. Gets positioned later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
    fillWindow();
}

//----------------------------------------------------------------

MenuControl&
MenuControl::instance()
{
    static MenuControl menu;
    return menu;
}

//----------------------------------------------------------------
//
// Fills in the window like this, just this once at init time.
// 
// Later, multiple calls to draw() just transfers the already
// filled window.
//
//    0123456789 123456789 1234
// 0  ┌───────────────────────┐
// 1  │ Control Menu          │
// 2  ├───────────────────────┤
// 3  │ [c] Configure         │
// 4  │ [p] Players           │
// 5  │ [t] Tables            │
// 6  │ [a] Pause             │
// 7  │ [r] Resume            │
// 8  │ [. or esc] Back       │
// 9  └───────────────────────┘
//
void
MenuControl::fillWindow()
{
    box(pWin_, 0, 0);

    // Horizontal separator below the title
    mvwhline(pWin_, 2, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, winSize_.cols - 1, ACS_RTEE);

    mvwaddstr(pWin_,  1, 2, "Control Menu");
    mvwaddstr(pWin_,  3, 2, "[c] Configure");
    mvwaddstr(pWin_,  4, 2, "[p] Players");
    mvwaddstr(pWin_,  5, 2, "[t] Tables");
    mvwaddstr(pWin_,  6, 2, "[a] Pause");
    mvwaddstr(pWin_,  7, 2, "[r] Resume");
    mvwaddstr(pWin_,  8, 2, "[. or esc] Back");
}

//----------------------------------------------------------------
//
// SurfaceManager wants our window size and more.
// This occurs in context of SurfaceManager::pushSurface()
// SurfaceManager informs us shortly of our screen position.
// See setLocation() below. 
//
LocationRequest
MenuControl::getLocationRequest() const
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
MenuControl::setLocation(WindowPosition pos)
{
    winPos_ = pos;   // MenuControl is fixed size, no resizing needed
    repos(winPos_);  // Just need to re-position it
}

//----------------------------------------------------------------

void
MenuControl::draw()
{
    LOG_TRACE("MenuControl::draw()");
    
    touchwin(pWin_);   // ncurses problem? Have to force content refresh.
    CuiUtils::transfer(pWin_);
}    

//----------------------------------------------------------------
//
// Override surface base class
//
bool
MenuControl::handleKey(int ch)
{
    bool handled = true;
    switch(ch)
    {
    case 'c': doConfigure();    break;
    case 'p': doPlayers();      break;
    case 't': doTables();       break;
    case 'a': doPause();        break;
    case 'r': doResume();       break;
    case '.':
    case 27 : back();           break;
    default : handled = false;  break;
    }
    return handled;
}

//----------------------------------------------------------------

void
MenuControl::doConfigure()
{
    SurfaceManager::instance().pushSurface(&MenuConfigure::instance());
}

//----------------------------------------------------------------

void
MenuControl::doPlayers()
{
    SurfaceManager::instance().pushSurface(&MenuPlayers::instance());
}

//----------------------------------------------------------------

void
MenuControl::doTables()
{
    SurfaceManager::instance().pushSurface(&MenuTable::instance());
}

//----------------------------------------------------------------

void
MenuControl::doPause()
{
}

//----------------------------------------------------------------

void
MenuControl::doResume()
{
}

//----------------------------------------------------------------

void
MenuControl::back()
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
