//----------------------------------------------------------------
//
// File: LayoutCrapsScreen.cpp
//
//----------------------------------------------------------------

#include <cui/LayoutCrapsScreen.h>
#include <cui/CuiUtils.h>
#include <ncurses.h>

using namespace Cui;

//----------------------------------------------------------------
//
// Draw outer most border and all the ScreenCrapsTable rectangles.
/*    
 ┌──────────────────────────────────────┬──────────────┐
 │              Header                  │    Dice      │
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
 └──────────────────────────────────────┴──────────────┘

  This layout is drawn once. Then the individual areas/rectangles are
  each their own window class and they will soon modify their boxing
  rectangle as necessary with junctions to align with internal element
  separators. The NavBar window class will draw itself and attach to the
  bottom of LayoutCrapsScreen.

 ├                                                     ┤
 │                        NavBar                       │
 └─────────────────────────────────────────────────────┘
*/

void
LayoutCrapsScreen::draw()
{
    using O = LayoutConsole;
    using C = LayoutCrapsScreen;

    // But stdscr might be larger than we want our Craps Table
    // So don't use box(), or wborder()
    // box(stdscr, 0, 0);  // 0,0 uses default chars vert and horiz char
    
    // Draw the border encapsulating the minimum size layout. This works
    // even if stdscr is larger than the minimum craps table size. Avoid
    // functions box() and wborder() which use stdscr dimensions
    // which are likely larger than the CrapsTableScreen since stdscr
    // is auto initialized to the terminal window size.

    mvhline(O::contentRowTop, O::contentColLeft,  ACS_HLINE, O::contentWidth);
    mvhline(O::contentRowBot, O::contentColLeft,  ACS_HLINE, O::contentWidth);
    mvvline(O::contentRowTop, O::contentColLeft,  ACS_VLINE, O::contentHeight);
    mvvline(O::contentRowTop, O::contentColRight, ACS_VLINE, O::contentHeight);

    // Now the four corners
    mvaddch(O::contentRowTop, O::contentColLeft,  ACS_ULCORNER);
    mvaddch(O::contentRowTop, O::contentColRight, ACS_URCORNER);
    mvaddch(O::contentRowBot, O::contentColLeft,  ACS_LLCORNER);
    mvaddch(O::contentRowBot, O::contentColRight, ACS_LRCORNER);
    
    // Horizontal lines
    mvhline(L::rollHistBorderTopRow,    L::rollHistBorderLeftCol,    ACS_HLINE, L::rollHistBorderRightCol    - L::rollHistBorderLeftCol    + 1);
    mvhline(L::playerAreaBorderTopRow,  L::playerAreaBorderLeftCol,  ACS_HLINE, L::playerAreaBorderRightCol  - L::playerAreaBorderLeftCol  + 1);
    mvhline(L::messageBorderTopRow,     L::messageBorderLeftCol,     ACS_HLINE, L::messageBorderRightCol     - L::messageBorderLeftCol     + 1);
    mvhline(L::houseBriefBorderTopRow,  L::houseBriefBorderLeftCol,  ACS_HLINE, L::houseBriefBorderRightCol  - L::houseBriefBorderLeftCol  + 1);
    mvhline(L::playerBriefBorderTopRow, L::playerBriefBorderLeftCol, ACS_HLINE, L::playerBriefBorderRightCol - L::playerBriefBorderLeftCol + 1);
    mvhline(L::navBarBorderTopRow,      L::navBarBorderLeftCol,      ACS_HLINE, L::navBarBorderRightCol      - L::navBarBorderLeftCol      + 1);

    // Vertical lines
    mvvline(L::animationBorderTopRow,   L::animationBorderLeftCol,   ACS_VLINE, L::animationBorderBotRow   - L::animationBorderTopRow   + 1);
    mvvline(L::houseBriefBorderTopRow,  L::houseBriefBorderLeftCol,  ACS_VLINE, L::houseBriefBorderBotRow  - L::houseBriefBorderTopRow  + 1);
    mvvline(L::playerBriefBorderTopRow, L::playerBriefBorderLeftCol, ACS_VLINE, L::playerBriefBorderBotRow - L::playerBriefBorderTopRow + 1);

    // Junctions on top border
    mvaddch(L::headerBorderTopRow, L::headerBorderRightCol, ACS_TTEE);

    // Junctions on left border
    mvaddch(L::rollHistBorderTopRow,   L::rollHistBorderLeftCol,   ACS_LTEE);
    mvaddch(L::playerAreaBorderTopRow, L::playerAreaBorderLeftCol, ACS_LTEE);
    mvaddch(L::messageBorderTopRow,    L::messageBorderLeftCol,    ACS_LTEE);
    mvaddch(L::messageBorderBotRow,    L::messageBorderLeftCol,    ACS_LTEE);

    // Junctions on right border
    mvaddch(L::houseBriefBorderTopRow,  L::houseBriefBorderRightCol,  ACS_RTEE);
    mvaddch(L::playerBriefBorderTopRow, L::playerBriefBorderRightCol, ACS_RTEE);
    mvaddch(L::navBarBorderTopRow,      L::navBarBorderRightCol,      ACS_RTEE);

    // Junctions at column split
    mvaddch(L::rollHistBorderTopRow,    L::rollHistBorderRightCol,   ACS_RTEE);
    mvaddch(L::playerAreaBorderTopRow,  L::playerAreaBorderRightCol, ACS_RTEE);
    mvaddch(L::messageBorderTopRow,     L::messageBorderRightCol,    ACS_PLUS);
    mvaddch(L::playerBriefBorderTopRow, L::playerBriefBorderLeftCol, ACS_LTEE);
    mvaddch(L::playerBriefBorderBotRow, L::playerBriefBorderLeftCol, ACS_BTEE);

    CuiUtils::transfer(stdscr);
}

//----------------------------------------------------------------
