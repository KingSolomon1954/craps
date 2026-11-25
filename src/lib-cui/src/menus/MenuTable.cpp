//----------------------------------------------------------------
//
// File: MenuTable.cpp
//
//----------------------------------------------------------------

#include <cui/menus/MenuTable.h>
#include <cui/managers/OperationManager.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/core/CuiUtils.h>
#include <gen/Logger.h>

using namespace Cui;

namespace {
using Op = OperationManager::TableOp;
}

//----------------------------------------------------------------

MenuTable::MenuTable()
    : MenuBase("MenuTable")
{
    // Create an initial WINDOW at location 0,0. Gets positioned later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
    fillWindow();
}

//----------------------------------------------------------------

MenuTable&
MenuTable::instance()
{
    static MenuTable menu;
    return menu;
}

//----------------------------------------------------------------
//
// Fills in the window like this, just this once at init time.
// 
// Later, multiple calls to draw() just transfers the already
// filled window.
//
//    0123456789 123456789 123
// 0  ┌──────────────────────┐
// 1  │ Table Menu           │
// 2  ├──────────────────────┤
// 3  │ [l] List Tables      │
// 4  │ [s] Switch to Table  │
// 5  │ [. or esc] Back      │
// 6  └──────────────────────┘
//
void
MenuTable::fillWindow()
{
    box(pWin_, 0, 0);

    // Horizontal separator below the title
    mvwhline(pWin_, 2, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, winSize_.cols - 1, ACS_RTEE);

    mvwaddstr(pWin_, 1, 2, "Table Menu");
    mvwaddstr(pWin_, 3, 2, "[l] List Tables");
    mvwaddstr(pWin_, 4, 2, "[s] Switch to Table");
    mvwaddstr(pWin_, 5, 2, "[. or esc] Back");
}

//----------------------------------------------------------------
//
// SurfaceManager wants our window size and more.
// This occurs in context of SurfaceManager::pushSurface()
// SurfaceManager informs us shortly of our screen position.
// See setLocation() below. 
//
LocationRequest
MenuTable::getLocationRequest() const
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
MenuTable::setLocation(WindowPosition pos)
{
    winPos_ = pos;   // MenuTable is fixed size, no resizing needed
    repos(winPos_);  // Just need to re-position it
}

//----------------------------------------------------------------

void
MenuTable::draw()
{
    LOG_TRACE("MenuTable::draw()");
    
    touchwin(pWin_);   // ncurses problem? Have to force content refresh.
    CuiUtils::transfer(pWin_);
}    

//----------------------------------------------------------------
//
// Override surface base class
//
bool
MenuTable::handleKey(int ch)
{
    bool handled = true;
    switch(ch)
    {
    case 'l': doListTables();        break;
    case 's': doSwitchToTable();     break;
    case '.':
    case 27 : back();                break;
    default : handled = false;       break;
    }
    return handled;
}

//----------------------------------------------------------------

void
MenuTable::doListTables()
{
    (void) OperationManager::instance().tableOperation(Op::List);
}

//----------------------------------------------------------------

void
MenuTable::doSwitchToTable()
{
    (void) OperationManager::instance().tableOperation(Op::Switch);
}

//----------------------------------------------------------------

void
MenuTable::back()
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
