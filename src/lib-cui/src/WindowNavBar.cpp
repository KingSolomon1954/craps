a//----------------------------------------------------------------
//
// File: WindowNavBar.cpp
//
//----------------------------------------------------------------

#include <cui/WindowNavBar.h>
#include <cui/CuiUtils.h>
#include <gen/Logger.h>
#include <ncurses.h>

using namespace Cui;

//----------------------------------------------------------------

WindowNavBar::WindowNavBar()
{
    pWindow_ = newwin(
        Layout::navHeight,
        Layout::navWidth,
        Layout::navTopRow,
        Layout::navLeftCol);
    
    assert(pWindow_ != nullptr);
}

//----------------------------------------------------------------

WindowWindowNavBar::~WindowNavBar()
{
    if (pWindow_ != nullptr)
    {
        delwin(pWindow_);
    }
}

//----------------------------------------------------------------

WindowWindowNavBar*
WindowWindowNavBar::instance()
{
    static WindowNavBar navBar;
    return &navBar;
}

//----------------------------------------------------------------
//
// Move window contents to ncurses virtual backing store
//
void
WindowNavBar::transfer();
{
    wnoutrefresh(pWindow_);
}

//----------------------------------------------------------------

void
WindowNavBar::draw()
{
    werase(pWindow_);
    drawBorders();
    drawStaticContent();
    populate();
    CuiUtils::transfer(pWindow_);
}

//----------------------------------------------------------------

void
WindowNavBar::drawBorders()
{
    drawExternalBorder();
    drawExternalJunctions();
    drawInternalBorders();
}

//----------------------------------------------------------------

void
WindowNavBar::drawExternalBorder()
{
    using C = LayoutConsole;

    // Top border line is already there from previous full screen drawing.
    // Leave it alone. We have no changes to it.
    
    // Horizontal line at bottom of screen
    mvhline(C::navBorderRowBot, C::navBorderColLeft, ACS_HLINE, C::navBorderWidth);

    // Two vertical lines at each side of the screen
    mvaddch(C::navBorderRowTop + 1, C::navBorderColLeft,   ACS_VLINE);
    mvaddch(C::navBorderRowTop + 1, C::navBorderColRight,  ACS_VLINE);

    // The bottom left and right cornners.
    mvaddch(C::navBorderRowBot, C::navBorderColLeft,  ACS_LLCORNER);
    mvaddch(C::navBorderRowBot, C::navBorderColRight, ACS_LRCORNER);
}

//----------------------------------------------------------------

void
WindowNavBar::drawExternalJunctions()
{
    using L = Layout;;
    using C = LayoutConsole;
    
    // Now replace the two top corners with "T's" to properly mate
    // with Message and Player Brief border lines.
    mvaddch(C::navBorderRowTop, C::navBorderColLeft,  ACS_LTEE);
    mvaddch(C::navBorderRowTop, C::navBorderColRight, ACS_RTEE);
}

//----------------------------------------------------------------

void
WindowNavBar::drawInternalBorders()
{
    // TODO
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
    mvwprintw(pWindow_, 0, 0, "Nav Bar here");
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
WindowNavBar::configure()
{
    // TODO
}

//----------------------------------------------------------------

void
WindowNavBar::clear()
{
    werase(pWindow_);
}

//----------------------------------------------------------------
