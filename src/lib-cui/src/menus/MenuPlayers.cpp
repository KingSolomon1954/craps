//----------------------------------------------------------------
//
// File: MenuPlayers.cpp
//
//----------------------------------------------------------------

#include <cui/menus/MenuPlayers.h>
#include <cui/managers/OperationManager.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/core/CuiUtils.h>
#include <gen/Logger.h>

using namespace Cui;

namespace {
using Op = OperationManager::PlayerOp;
}

//----------------------------------------------------------------

MenuPlayers::MenuPlayers()
    : MenuBase("MenuPlayers")
{
    // Create an initial WINDOW at location 0,0. Gets positioned later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
    fillWindow();
}

//----------------------------------------------------------------

MenuPlayers&
MenuPlayers::instance()
{
    static MenuPlayers menu;
    return menu;
}

//----------------------------------------------------------------
//
// Fills in the window like this, just this once at init time.
// 
// Later, multiple calls to draw() just transfers the already
// filled window.
//
//    0123456789 123456789 12345
// 0  ┌────────────────────────┐
// 1  │ Player Menu            │
// 2  ├────────────────────────┤
// 3  │ [l] List Players       │
// 4  │ [s] Switch To Player   │
// 5  │ [r] Rename Player      │
// 6  │ [c] Create New Player  │
// 7  │ [d] Delete Player      │
// 8  │ [b] List Buddies       │
// 9  │ [j] Buddy Joins Table  │
// 10 │ [k] Buddy Leaves Table │
// 11 │ [. or esc] Back        │
// 12 └────────────────────────┘
//
void
MenuPlayers::fillWindow()
{
    box(pWin_, 0, 0);

    // Horizontal separator below the title
    mvwhline(pWin_, 2, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, winSize_.cols - 1, ACS_RTEE);

    mvwaddstr(pWin_,  1, 2, "Player Menu");
    mvwaddstr(pWin_,  3, 2, "[l] List Players");
    mvwaddstr(pWin_,  4, 2, "[s] Switch to Player");
    mvwaddstr(pWin_,  5, 2, "[r] Rename Player");
    mvwaddstr(pWin_,  6, 2, "[c] Create New Player");
    mvwaddstr(pWin_,  7, 2, "[d] Delete Player");
    mvwaddstr(pWin_,  8, 2, "[b] List Buddies");
    mvwaddstr(pWin_,  9, 2, "[j] Buddy Joins Table");
    mvwaddstr(pWin_, 10, 2, "[k] Buddy Leaves Table");
    mvwaddstr(pWin_, 11, 2, "[. or esc] Back");
}

//----------------------------------------------------------------
//
// SurfaceManager wants our window size and more.
// This occurs in context of SurfaceManager::pushSurface()
// SurfaceManager informs us shortly of our screen position.
// See setLocation() below. 
//
LocationRequest
MenuPlayers::getLocationRequest() const
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
MenuPlayers::setLocation(WindowPosition pos)
{
    winPos_ = pos;   // MenuPlayers is fixed size, no resizing needed
    repos(winPos_);  // Just need to re-position it
}

//----------------------------------------------------------------

void
MenuPlayers::draw()
{
    LOG_TRACE("MenuPlayers::draw()");
    
    touchwin(pWin_);   // ncurses problem? Have to force content refresh.
    CuiUtils::transfer(pWin_);
}    

//----------------------------------------------------------------
//
// Override surface base class
//
bool
MenuPlayers::handleKey(int ch)
{
    bool handled = true;
    switch(ch)
    {
    case 'l': doListPlayers();       break;
    case 's': doSwitchToPlayer();    break;
    case 'r': doRenamePlayer();      break;
    case 'c': doCreatePlayer();      break;
    case 'd': doDeletePlayer();      break;
    case 'b': doListBuddies();       break;
    case 'j': doBuddyJoinsTable();   break;
    case 'k': doBuddyLeavesTable();  break;
    case '.':
    case 27 : back();                break;
    default : handled = false;       break;
    }
    return handled;
}

//----------------------------------------------------------------

void
MenuPlayers::doListPlayers()
{
    (void) OperationManager::instance().playerOperation(Op::List);
}

//----------------------------------------------------------------

void
MenuPlayers::doSwitchToPlayer()
{
    (void) OperationManager::instance().playerOperation(Op::Switch);
}

//----------------------------------------------------------------

void
MenuPlayers::doRenamePlayer()
{
    (void) OperationManager::instance().playerOperation(Op::Rename);
}

//----------------------------------------------------------------

void
MenuPlayers::doCreatePlayer()
{
    (void) OperationManager::instance().playerOperation(Op::Create);
}

//----------------------------------------------------------------

void
MenuPlayers::doDeletePlayer()
{
    (void) OperationManager::instance().playerOperation(Op::Delete);
}

//----------------------------------------------------------------

void
MenuPlayers::doListBuddies()
{
    (void) OperationManager::instance().playerOperation(Op::ListBuddies);
}

//----------------------------------------------------------------

void
MenuPlayers::doBuddyJoinsTable()
{
    (void) OperationManager::instance().playerOperation(Op::BuddyJoins);
}

//----------------------------------------------------------------

void
MenuPlayers::doBuddyLeavesTable()
{
    (void) OperationManager::instance().playerOperation(Op::BuddyLeaves);
}

//----------------------------------------------------------------

void
MenuPlayers::back()
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
