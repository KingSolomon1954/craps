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
LayoutCrapsScreen::draw(WINDOW* pWin)
{
    using O = LayoutConsole;
    using C = LayoutCrapsScreen;

    box(pWin, 0, 0);  // 0,0 uses default chars vert and horiz char
    
    // Horizontal lines
    mvwhline(pWin_, L::rollHistBorderTopRow,    L::rollHistBorderLeftCol,    0, L::rollHistBorderRightCol    - L::rollHistBorderLeftCol    + 1);
    mvwhline(pWin_, L::playerAreaBorderTopRow,  L::playerAreaBorderLeftCol,  0, L::playerAreaBorderRightCol  - L::playerAreaBorderLeftCol  + 1);
    mvwhline(pWin_, L::messageBorderTopRow,     L::messageBorderLeftCol,     0, L::messageBorderRightCol     - L::messageBorderLeftCol     + 1);
    mvwhline(pWin_, L::houseBriefBorderTopRow,  L::houseBriefBorderLeftCol,  0, L::houseBriefBorderRightCol  - L::houseBriefBorderLeftCol  + 1);
    mvwhline(pWin_, L::playerBriefBorderTopRow, L::playerBriefBorderLeftCol, 0, L::playerBriefBorderRightCol - L::playerBriefBorderLeftCol + 1);
    mvwhline(pWin_, L::navBarBorderTopRow,      L::navBarBorderLeftCol,      0, L::navBarBorderRightCol      - L::navBarBorderLeftCol      + 1);

    // Vertical lines
    mvwvline(pWin, L::animationBorderTopRow,   L::animationBorderLeftCol,   0, L::animationBorderBotRow   - L::animationBorderTopRow   + 1);
    mvwvline(pWin, L::houseBriefBorderTopRow,  L::houseBriefBorderLeftCol,  0, L::houseBriefBorderBotRow  - L::houseBriefBorderTopRow  + 1);
    mvwvline(pWin, L::playerBriefBorderTopRow, L::playerBriefBorderLeftCol, 0, L::playerBriefBorderBotRow - L::playerBriefBorderTopRow + 1);

    // Junctions on top border
    mvwaddch(pWin_, L::headerBorderTopRow, L::headerBorderRightCol, ACS_TTEE);

    // Junctions on left border
    mvwaddch(pWin_, L::rollHistBorderTopRow,   L::rollHistBorderLeftCol,   ACS_LTEE);
    mvwaddch(pWin_, L::playerAreaBorderTopRow, L::playerAreaBorderLeftCol, ACS_LTEE);
    mvwaddch(pWin_, L::messageBorderTopRow,    L::messageBorderLeftCol,    ACS_LTEE);
    mvwaddch(pWin_, L::messageBorderBotRow,    L::messageBorderLeftCol,    ACS_LTEE);

    // Junctions on right border
    mvwaddch(pWin_, L::houseBriefBorderTopRow,  L::houseBriefBorderRightCol,  ACS_RTEE);
    mvwaddch(pWin_, L::playerBriefBorderTopRow, L::playerBriefBorderRightCol, ACS_RTEE);
    mvwaddch(pWin_, L::navBarBorderTopRow,      L::navBarBorderRightCol,      ACS_RTEE);

    // Junctions at column split
    mvwaddch(pWin_, L::rollHistBorderTopRow,    L::rollHistBorderRightCol,   ACS_RTEE);
    mvwaddch(pWin_, L::playerAreaBorderTopRow,  L::playerAreaBorderRightCol, ACS_RTEE);
    mvwaddch(pWin_, L::messageBorderTopRow,     L::messageBorderRightCol,    ACS_PLUS);
    mvwaddch(pWin_, L::playerBriefBorderTopRow, L::playerBriefBorderLeftCol, ACS_LTEE);
    mvwaddch(pWin_, L::playerBriefBorderBotRow, L::playerBriefBorderLeftCol, ACS_BTEE);

    CuiUtils::transfer(stdscr);
}

//----------------------------------------------------------------
