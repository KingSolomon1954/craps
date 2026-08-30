//----------------------------------------------------------------
//
// File: LayoutCrapsScreen.cpp
//
//----------------------------------------------------------------

#include <cui/LayoutCrapsScreen.h>
#include <cui/LayoutConsole.h>
#include <cui/LayoutPlayerArea.h>
#include <cui/CuiUtils.h>
#include <stdexcept>

using namespace Cui;

//----------------------------------------------------------------
//
// pWin is owned by ScreenCrapsTable. In this class we borrow it 
// to draw borders. Several Panels call into here later to fix 
// up borders surrounding their Panel.
//
void
LayoutCrapsScreen::init(WINDOW* pWin)
{
    if (pWin_)
    {
        throw std::logic_error("LayoutWindow already initialized");
    }
    pWin_ = pWin;
}

//----------------------------------------------------------------

LayoutCrapsScreen&
LayoutCrapsScreen::instance()
{
    if (!pWin_)
    {
        throw std::logic_error("LayoutCrapsScreen::init() must be called first");
    }
    
    // Only reached, therefore only constructed, if pWin_ is set
    static LayoutCrapsScreen lcs;
    return lcs;
}

//----------------------------------------------------------------
//
// Draw borders surrounding all the ScreenCrapsTable Panels.
/*    
   ┌──────────────────────────────────────┬──────────────┐
   │            Title Bar                 │    Dice      │
   ├──────────────────────────────────────┤  Animation   │
   │           Roll History               │              │
   ├──────────────────────────────────────┤              │
   │                                      │              │
   │          Player Area                 │              │
   │                                      │              │
   ├──────────────────────────────────────┼──────────────┤
   │             Messages                 │ House Brief  │
   │                                      ├──────────────┤
   │                                      │ Player Brief │
   ├──────────────────────────────────────┴──────────────┤                                                     ┤
   │                        NavBar                       │
   └─────────────────────────────────────────────────────┘

   This layout is drawn once. Later, Panels will call
   into this class to touch up their surrounding borders 
   to align with internal borders they might have.
*/

void
LayoutCrapsScreen::draw()
{
    using C = LayoutConsole;
    using L = LayoutCrapsScreen;

    box(pWin_, 0, 0);  // 0,0 uses default chars vert and horiz char
    
    // Horizontal lines
    mvwhline(pWin_, L::rollHistBorderTopRow,    L::rollHistBorderLeftCol,    0, L::rollHistBorderWidth);
    mvwhline(pWin_, L::playerAreaBorderTopRow,  L::playerAreaBorderLeftCol,  0, L::playerAreaBorderWidth);
    mvwhline(pWin_, L::messageBorderTopRow,     L::messageBorderLeftCol,     0, L::messageBorderWidth);
    mvwhline(pWin_, L::houseBriefBorderTopRow,  L::houseBriefBorderLeftCol,  0, L::houseBriefBorderWidth);
    mvwhline(pWin_, L::playerBriefBorderTopRow, L::playerBriefBorderLeftCol, 0, L::playerBriefBorderWidth);
    mvwhline(pWin_, C::navBorderTopRow,         C::navBorderLeftCol,         0, C::navBorderWidth);

    // Vertical lines
    mvwvline(pWin_, L::animationBorderTopRow,   L::animationBorderLeftCol,   0, L::animationBorderHeight);
    mvwvline(pWin_, L::houseBriefBorderTopRow,  L::houseBriefBorderLeftCol,  0, L::houseBriefBorderHeight);
    mvwvline(pWin_, L::playerBriefBorderTopRow, L::playerBriefBorderLeftCol, 0, L::playerBriefBorderHeight);

    // Junctions on top border
    mvwaddch(pWin_, L::titleBorderTopRow, L::titleBorderRightCol, ACS_TTEE);

    // Junctions on left border
    mvwaddch(pWin_, L::rollHistBorderTopRow,   L::rollHistBorderLeftCol,   ACS_LTEE);
    mvwaddch(pWin_, L::playerAreaBorderTopRow, L::playerAreaBorderLeftCol, ACS_LTEE);
    mvwaddch(pWin_, L::messageBorderTopRow,    L::messageBorderLeftCol,    ACS_LTEE);

    // Junctions on right border
    mvwaddch(pWin_, L::houseBriefBorderTopRow,  L::houseBriefBorderRightCol,  ACS_RTEE);
    mvwaddch(pWin_, L::playerBriefBorderTopRow, L::playerBriefBorderRightCol, ACS_RTEE);

    // Junctions at column split
    mvwaddch(pWin_, L::rollHistBorderTopRow,    L::rollHistBorderRightCol,   ACS_RTEE);
    mvwaddch(pWin_, L::playerAreaBorderTopRow,  L::playerAreaBorderRightCol, ACS_RTEE);
    mvwaddch(pWin_, L::messageBorderTopRow,     L::messageBorderRightCol,    ACS_PLUS);
    mvwaddch(pWin_, L::playerBriefBorderTopRow, L::playerBriefBorderLeftCol, ACS_LTEE);
    mvwaddch(pWin_, L::playerBriefBorderBotRow, L::playerBriefBorderLeftCol, ACS_BTEE);

    // Navbar junctions
    mvwaddch(pWin_, C::navBorderTopRow, C::navBorderLeftCol,  ACS_LTEE);
    mvwaddch(pWin_, C::navBorderTopRow, C::navBorderRightCol, ACS_RTEE);

    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------
//
// Fix up junctions for outer borders for all players view
//
void
LayoutCrapsScreen::drawExternalJunctionsAllPlayers()
{
    using L = LayoutCrapsScreen;
    using A = LayoutAllPlayers;
    
    // Has 4 junctions to apply
    mvwaddch(pWin_, L::playerAreaBorderTopRow, A::col2, ACS_TTEE);
    mvwaddch(pWin_, L::playerAreaBorderTopRow, A::col3, ACS_TTEE);
    mvwaddch(pWin_, L::playerAreaBorderBotRow, A::col2, ACS_BTEE);
    mvwaddch(pWin_, L::playerAreaBorderBotRow, A::col3, ACS_BTEE);
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------
//
// Restores player area junctions to a smooth rectangle
//
void
LayoutCrapsScreen::eraseExternalJunctionsAllPlayers()
{
    using L = LayoutCrapsScreen;
    using A = LayoutAllPlayers;
    
    // Has 4 junctions to restore
    mvwaddch(pWin_, L::playerAreaBorderTopRow, A::col2, ACS_HLINE);
    mvwaddch(pWin_, L::playerAreaBorderTopRow, A::col3, ACS_HLINE);
    mvwaddch(pWin_, L::playerAreaBorderBotRow, A::col2, ACS_HLINE);
    mvwaddch(pWin_, L::playerAreaBorderBotRow, A::col3, ACS_HLINE);
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------
//
// Fix up junctions for outer borders for one player view
//
void
LayoutCrapsScreen::drawExternalJunctionsOnePlayer()
{
    using L = LayoutCrapsScreen;
    using O = LayoutOnePlayer;

    // Top border
    mvwaddch(pWin_, L::playerAreaBorderTopRow, O::col2, ACS_TTEE);
    mvwaddch(pWin_, L::playerAreaBorderTopRow, O::col3, ACS_TTEE);
    mvwaddch(pWin_, L::playerAreaBorderTopRow, O::col4, ACS_TTEE);
    mvwaddch(pWin_, L::playerAreaBorderTopRow, O::col5, ACS_TTEE);
    mvwaddch(pWin_, L::playerAreaBorderTopRow, O::col6, ACS_TTEE);

    // Bottom border
    mvwaddch(pWin_, L::playerAreaBorderBotRow, O::lineBetSplitCol, ACS_BTEE);

    // Left border    
    mvwaddch(pWin_, O::rowField,    L::playerAreaBorderLeftCol, ACS_LTEE);
    mvwaddch(pWin_, O::rowCraps,    L::playerAreaBorderLeftCol, ACS_LTEE);
    mvwaddch(pWin_, O::rowLineBets, L::playerAreaBorderLeftCol, ACS_LTEE);
    
    // Right border    
    mvwaddch(pWin_, O::rowField,    L::playerAreaBorderRightCol, ACS_RTEE);
    mvwaddch(pWin_, O::rowCraps,    L::playerAreaBorderRightCol, ACS_RTEE);
    mvwaddch(pWin_, O::rowLineBets, L::playerAreaBorderRightCol, ACS_RTEE);
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------
//
// Restores table area junctions to a smooth square
//
void
LayoutCrapsScreen::eraseExternalJunctionsOnePlayer()
{
    using L = LayoutCrapsScreen;
    using O = LayoutOnePlayer;

    // Top border
    mvwaddch(pWin_, L::playerAreaBorderTopRow, O::col2, ACS_HLINE);
    mvwaddch(pWin_, L::playerAreaBorderTopRow, O::col3, ACS_HLINE);
    mvwaddch(pWin_, L::playerAreaBorderTopRow, O::col4, ACS_HLINE);
    mvwaddch(pWin_, L::playerAreaBorderTopRow, O::col5, ACS_HLINE);
    mvwaddch(pWin_, L::playerAreaBorderTopRow, O::col6, ACS_HLINE);

    // Bottom border
    mvwaddch(pWin_, L::playerAreaBorderBotRow, O::lineBetSplitCol, ACS_HLINE);

    // Left border    
    mvwaddch(pWin_, O::rowField,    L::playerAreaBorderLeftCol, ACS_VLINE);
    mvwaddch(pWin_, O::rowCraps,    L::playerAreaBorderLeftCol, ACS_VLINE);
    mvwaddch(pWin_, O::rowLineBets, L::playerAreaBorderLeftCol, ACS_VLINE);
    
    // Right border    
    mvwaddch(pWin_, O::rowField,    L::playerAreaBorderRightCol, ACS_VLINE);
    mvwaddch(pWin_, O::rowCraps,    L::playerAreaBorderRightCol, ACS_VLINE);
    mvwaddch(pWin_, O::rowLineBets, L::playerAreaBorderRightCol, ACS_VLINE);
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------
