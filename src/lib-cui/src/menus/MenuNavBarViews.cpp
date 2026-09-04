//----------------------------------------------------------------
//
// File: MenuNavBarViews.cpp
//
//----------------------------------------------------------------

#include <cui/menus/MenuNavBarViews.h>
#include <cui/panels/WindowPlayerArea.h>
#include <cui/SurfaceManager.h>
#include <cui/CuiUtils.h>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

MenuNavBarViews::MenuNavBarViews()
{
    createWindow();
    registerWindow();
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

void
MenuNavBarViews::createWindow()
{
    using L = Layout;    
    newWindow(L::winHeight, L::winWidth,
              L::winStartY, L::winStartX,
              "MenuNaveBarViews");
    SurfaceManager::instance().registerForShutdown(this);
}

//----------------------------------------------------------------

void
MenuNavBarViews::registerWindow()
{
    SurfaceManager::instance().registerForShutdown(this);
    surfaceName_ = "MenuNavBarViews";
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
// 6  │ [esc] Back            │
// 7  └───────────────────────┘
//
void
MenuNavBarViews::fillWindow()
{
    using L = Layout;
    
    // Draw our border.
    box(pWin_, 0, 0);

    // Draw the horizontal separator below the title.
    mvwhline(pWin_, 2, 1, ACS_HLINE, L::winWidth - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, L::winWidth - 1, ACS_RTEE);

    // Static contents. The border occupies row 0/10 and column 0/21.
    mvwaddstr(pWin_, 1, 2, "View Menu");

    mvwaddstr(pWin_, 3, 2, "[A] All Players");
    mvwaddstr(pWin_, 4, 2, "[N] One Player (next)");
    mvwaddstr(pWin_, 5, 2, "[P] One PLayer (prev)");
    mvwaddstr(pWin_, 6, 2, "[esc] Back");
}

//----------------------------------------------------------------

void
MenuNavBarViews::draw()
{
    // Just reuse already filled window over and over
    CuiUtils::transfer(pWin_);
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
    case 27 : back();          break;
    default : handled = false; break;
    }
    return handled;
}

//----------------------------------------------------------------

void
MenuNavBarViews::allPlayers()
{
    WindowPlayerArea::instance().allPlayers();
    setOperationResult(OperationResult::Success);
    SurfaceManager::instance().popSurfaces();
}

//----------------------------------------------------------------

void
MenuNavBarViews::nextPlayer()
{
    WindowPlayerArea::instance().nextPlayer();
    setOperationResult(OperationResult::Success);
    SurfaceManager::instance().popSurfaces();
}

//----------------------------------------------------------------

void
MenuNavBarViews::prevPlayer()
{
    WindowPlayerArea::instance().prevPlayer();
    setOperationResult(OperationResult::Success);
    SurfaceManager::instance().popSurfaces();
}

//----------------------------------------------------------------

void
MenuNavBarViews::back()
{
    // Set our own state in base class to reflect cancel.
    // Also informs parent surfaces of the state of operation.
    // In turn, parent menus can decide if they are skipped
    // when unwinding the menu stack.
    //
    setOperationResult(OperationResult::Cancel);
    SurfaceManager::instance().popSurfaces();
}

//----------------------------------------------------------------
