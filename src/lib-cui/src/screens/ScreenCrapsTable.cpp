//----------------------------------------------------------------
//
// File: ScreenCrapsTable.cpp
//
//----------------------------------------------------------------

#include <cui/screens/ScreenCrapsTable.h>
#include <cui/ConsoleManager.h>
#include <cui/layouts/LayoutCrapsScreen.h>
// #include <cui/panles/WindowTitleBar.h>
// #include <cui/panles/WindowRollHistory.h>
// #include <cui/panles/WindowWindowPlayerArea.h>
// #include <cui/panles/WindowMessages.h>
// #include <cui/panles/WindowAnimation.h>
// #include <cui/panles/WindowHouseBrief.h>
// #include <cui/panles/WindowPlayerBrief.h>
#include <cui/panels/WindowNavBar.h>
#include <gen/Logger.h>

using namespace Cui;

//----------------------------------------------------------------

ScreenCrapsTable::ScreenCrapsTable()
{
    LOG_TRACE("Entered ScreenCrapsTable::ctor()");

    using L = LayoutCrapsScreen;
    newWindow(L::height, L::width,          // In base class
              L::topRow, L::leftCol,
              "ScreenCrapsTable");
    LayoutCrapsScreen::init(pWin_);
    
    LOG_TRACE("Leaving ScreenCrapsTable::ctor()");
}

//----------------------------------------------------------------

void
ScreenCrapsTable::releaseNcursesResources()
{
//    WindowTitleBar::instance().releaseNcursesResources();
//    WindowRollHistory::instance().releaseNcursesResources();
//    WindowPlayerArea::instance().releaseNcursesResources();
//    WindowMessages::instance().releaseNcursesResources();
//    WindowAnimation::instance().releaseNcursesResources();
//    WindowHouseBrief::instance().releaseNcursesResources();
//    WindowPlayerBrief::instance().releaseNcursesResources();
//    
    SurfaceBase::releaseNcursesResources();  // Invoke base class implementation
}

//----------------------------------------------------------------

ScreenCrapsTable&
ScreenCrapsTable::instance()
{
    static ScreenCrapsTable screenCrapsTable;
    return screenCrapsTable;
}

//----------------------------------------------------------------
//
// External entry point to draw screen.
// 
// Each window draw() below does its own transfer()
// Caller is responsible for eventual doupdate()
//
void
ScreenCrapsTable::draw()  // Override
{
    werase(pWin_);
    
    LayoutCrapsScreen::instance().draw();

//     WindowTitleBar::instance().draw();
//     WindowRollHistory::instance().draw();
//     WindowPlayerArea::instance().draw();
//     WindowMessages::instance().draw();
//     WindowAnimation::instance().draw();
//     WindowHouseBrief::instance().draw();
//     WindowPlayerBrief::instance().draw();
    drawNavBar();
}

//----------------------------------------------------------------

void
ScreenCrapsTable::drawNavBar()
{
    auto nav = WindowNavBar::instance();
    
    nav.clear();
    nav.configure(
    "[C] Control [B] BETTING [V] View [S] Stats | [H] Help [Q] Quit");
    nav.draw();
}

//----------------------------------------------------------------

void
ScreenCrapsTable::onAttach(SurfaceBase* pParent)
{
    LOG_TRACE("ScreenCrapsTable::onAttach()");
    (void)pParent;

    // TODO 
    // ConsoleManager::pushSurface(MenuBetting::instance());
}

//----------------------------------------------------------------
//
// ScreenCrapsTable does not process keys directly. MenuBetting owns
// and handles keyboard processing while this surface is active.
// Needed here to satisfy Surface pure virtual interface.
//
void
ScreenCrapsTable::handleKey(int ch)
{
    (void)ch;
    LOG_TRACE("ScreenCrapsTable::handleKey() should "
              "not get here:(" + std::to_string(ch) + ")");
}

//----------------------------------------------------------------



#if 0

// Not currently used. Just here a while for reference.

//----------------------------------------------------------------
//
// Basically undo what was done during onAttach()
//
void
ScreenCrapsTable::onDetach()
{
    LOG_TRACE("ScreenCrapsTable::onDetach()");
    // TODO
}

//----------------------------------------------------------------
//
// Stop timers if any and so on.
//
void
ScreenCrapsTable::onPause()
{
    LOG_TRACE("ScreenCrapsTable::onPause()");
    // TODO
}

//----------------------------------------------------------------
//
// Start/restart timers if any and so on.
//
void
ScreenCrapsTable::onResume()
{
    LOG_TRACE("ScreenCrapsTable::onResume()");
    // TODO
}

#endif

#if 0

//----------------------------------------------------------------
//
// Interpret single-char menu actions
//
void
ScreenCrapsTable::handleMenuInput(int ch)
{
    if (ch == ' ')
    {
        // TODO: push/toggle menu screen
        return;
    }
    switch(activeMenu_)
    {
    case Menus::Betting:  menuInputBetting(ch); break;
    case Menus::Stats:    menuInputStats(ch);   break;
    }
}

//----------------------------------------------------------------
//
// Gather then interpret command line input
//

std::string lineBuffer_;

void
ScreenCrapsTable::handleLineInput(int ch)
{
    if (ch == '\n')
    {
        processLineBuffer();
        lineBuffer_.clear();
        drawInputPrompt();
    } 
    else if (ch == KEY_BACKSPACE || ch == 127)
    {
        if (!lineBuffer_.empty())
        {
            lineBuffer_.pop_back();
        }
        drawInputPrompt();
    }
    else if (isprint(ch))
    {
        lineBuffer_.push_back(static_cast<char>(ch));
        drawInputPrompt();
    }
}

//----------------------------------------------------------------
//
// Gather then interpret command line input
//
void
ScreenCrapsTable::handleAmountInput(int ch)
{
    // TODO add left/right arrow keys and deletion under cursor
    if (ch == '\n')
    {
        processLineBuffer();
        lineBuffer_.clear();
        drawInputPrompt();
    }
    else if (ch == 'q')
    {
        setupQuickBet();
    }
    else if (ch == 'a')
    {
        setupAutoFill();
    }
    else if (ch == KEY_BACKSPACE || ch == 127)
    {
        if (!lineBuffer_.empty())
        {
            lineBuffer_.pop_back();
        }
        drawInputPrompt();
    }
    else if (isprint(ch))
    {
        lineBuffer_.push_back(static_cast<char>(ch));
        drawInputPrompt();
    }
}

//----------------------------------------------------------------

void
ScreenCrapsTable::drawInputPrompt()
{
//   werase(w_.command);
//    mvwprintw(w_.command, 0, 0, "> %s", lineBuffer_.c_str());
//    wrefresh(w_.command);
}

//----------------------------------------------------------------

#include <controller/CrapsGame.h>

void
ScreenCrapsTable::menuInputBetting(int ch)
{
//    LOG_TRACE("ScreenCrapsTable::menuInputBetting()");
    if (ch == 'b')
    {
        // back to previous menu
        view_.popScreen();
    }
    if (ch == 'q')
    {
        Ctrl::CrapsGame::instance()->terminateApp();
    }
}

//----------------------------------------------------------------

void
ScreenCrapsTable::menuInputStats(int ch)
{
    if (ch == 'b')
    {
        // back to previous menu 
        view_.popScreen();
    }
    if (ch == 'q')
    {
        Ctrl::CrapsGame::instance()->terminateApp();
    }
}

#endif
