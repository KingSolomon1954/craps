//----------------------------------------------------------------
//
// File: WindowNavBar.cpp
//
//----------------------------------------------------------------

#include <cui/panels/WindowNavBar.h>
#include <cui/CuiUtils.h>
#include <cui/layouts/LayoutConsole.h>
#include <cui/SurfaceManager.h>
#include <gen/Logger.h>
#include <ncurses.h>

using namespace Cui;

//----------------------------------------------------------------

WindowNavBar::WindowNavBar()
{
    createWindow();
    registerWindow();
}

//----------------------------------------------------------------

void
WindowNavBar::createWindow()
{
    newWindow(Layout::navHeight, Layout::navWidth,          // In base class
              Layout::navTopRow, Layout::navLeftCol,
              "WindowNavBar");
}


//----------------------------------------------------------------

void
WindowNavBar::registerWindow()
{
    SurfaceManager::instance().registerForShutdown(this);
    surfaceName_ = "MenuBetting";
}

//----------------------------------------------------------------

WindowNavBar&
WindowNavBar::instance()
{
    static WindowNavBar navBar;
    return navBar;
}

//----------------------------------------------------------------

void
WindowNavBar::draw()
{
    werase(pWin_);
    drawStaticContent();
    populate();
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------
//
// Draw static field contents
//
void
WindowNavBar::drawStaticContent()
{
    // TODO
}

//----------------------------------------------------------------
//
// Updates all dynamic field content.
//
void
WindowNavBar::populate()
{
    mvwprintw(pWin_, 0, 0, "Nav Bar here");
    // Updates all dynamic fields in this view.
    // TODO
    // updatePassLineBets()
    // updateFieldBets()
    // updateFieldBets()
    // updateXxx()
    // ...
}

//----------------------------------------------------------------
//
// Allow caller to specify the content of the nav bar window.
//
void
WindowNavBar::configure(const std::string& textLine)
{
    // TODO
}

//----------------------------------------------------------------

void
WindowNavBar::clear()
{
    werase(pWin_);
}

//----------------------------------------------------------------
//
// Returns true if key was processed, otherwise false.
//
bool
WindowNavBar::handleKey(int ch)
{
    bool handled = true;
    switch(ch)
    {
    case 'C': doControl();     break;
    case 'B': doBetting();     break;
    case 'V': doView();        break;
    case 'S': doStats();       break;
    case 'H': doHelp();        break;
    case 'Q': doQuit();        break;
    default : handled = false; break;
    }
    return handled;
}

//----------------------------------------------------------------

void
WindowNavBar::doControl()
{
    // TODO
}

//----------------------------------------------------------------

void
WindowNavBar::doBetting()
{
    // TODO
}

//----------------------------------------------------------------

void
WindowNavBar::doView()
{
    // TODO
}

//----------------------------------------------------------------

void
WindowNavBar::doStats()
{
    // TODO
}

//----------------------------------------------------------------

void
WindowNavBar::doHelp()
{
    // TODO
}

//----------------------------------------------------------------

void
WindowNavBar::doQuit()
{
    // TODO
}

//----------------------------------------------------------------


