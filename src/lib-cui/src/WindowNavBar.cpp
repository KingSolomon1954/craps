//----------------------------------------------------------------
//
// File: WindowNavBar.cpp
//
//----------------------------------------------------------------

#include <cui/WindowNavBar.h>
#include <cui/CuiUtils.h>
#include <cui/LayoutConsole.h>
#include <gen/Logger.h>
#include <ncurses.h>

using namespace Cui;

//----------------------------------------------------------------

WindowNavBar::WindowNavBar()
{
    newWindow(Layout::navHeight, Layout::navWidth,          // In base class
              Layout::navTopRow, Layout::navLeftCol,
              "WindowNavBar");
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
    switch(ch)
    {
    case 'C': doControl(); return true; break;
    case 'B': doBetting(); return true; break;
    case 'V': doView();    return true; break;
    case 'S': doStats();   return true; break;
    case 'H': doHelp();    return true; break;
    case 'Q': doQuit();    return true; break;
    default:  return false;
    }
    return false;
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


