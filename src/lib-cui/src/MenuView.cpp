//----------------------------------------------------------------
//
// File: MenuView.cpp
//
//----------------------------------------------------------------

#include <cui/MenuView.h>
#include <cstring>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

MenuView::MenuView()
{
    createWindow();
    fillWindow();
}

//----------------------------------------------------------------

MenuView&
MenuView::instance()
{
    static MenuView menu;
    return menu;
}

//----------------------------------------------------------------

MenuView::createWindow()
{
    using L = Layout;
    pWin_ = newwin(L::winHeight, L::winWidth, L::winStartY, L::winStartX);
    
    if (pWin_ == nullptr)
    {
        throw std::runtime_error(
            "Unable to create ncurses MenuView window");
    }
}

//----------------------------------------------------------------

MenuView::~MenuView()
{
    // pWin_ is delwin() in MenuBase
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
MenuView::fillWindow()
{
    using L = Layout;
    
    // Draw our border.
    box(pWin_, 0, 0);

    // Draw the horizontal separator below the title.
    mvwhline(pWin_, 2, 1, ACS_HLINE, L::winWidth - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, L::winWidth - 1, ACS_RTEE);

    // Static contents. The border occupies row 0/10 and column 0/21.
    mvwaddstr(win, 1, 2, "View Menu");

    mvwaddstr(pWin_, 3, 2, "[A] All Players");
    mvwaddstr(pWin_, 4, 2, "[N] One Player (next)");
    mvwaddstr(pWin_, 5, 2, "[P] One PLayer (prev)");
    mvwaddstr(pWin_, 6, 2, "[esc] Back");
}

//----------------------------------------------------------------

void
MenuPivot::draw()
{
    // Just reuse already filled window over and over
    CuiUtils::transfer(pWin_);
}    

//----------------------------------------------------------------
//
// Override surface base class
//
void
MenuPivot::handleKey(int ch)
{
    switch(ch)
    {
    case 'A': allPlayers(); break;
    case 'N': nextPlayer(); break;
    case 'P': prevPlayer(); break;
    case 27:  back();       break;
    }
}

//----------------------------------------------------------------

void
MenuView::allPlayers()
{
    ScreenCrapsTable::instance().setAllPlayersView();
    setOperationResult(OperationResult::success);
    ConsoleManager::popSurfaces();
}

//----------------------------------------------------------------

void
MenuView::nextPlayer()
{
    ScreenCrapsTable::instance().setNextPlayerView();
    setOperationResult(OperationResult::success);
    ConsoleManager::popSurfaces();
}

//----------------------------------------------------------------

void
MenuView::nextPlayer()
{
    ScreenCrapsTable::instance().setPrevPlayerView();
    setOperationResult(OperationResult::success);
    ConsoleManager::popSurfaces();
}

//----------------------------------------------------------------

void
MenuView::back()
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
