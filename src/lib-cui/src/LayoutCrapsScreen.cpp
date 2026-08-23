//----------------------------------------------------------------
//
// File: LayoutCrapsScreen.cpp
//
//----------------------------------------------------------------

#include <cui/LayoutCrapsScreen.h>
#include <ncurses.h>

using namespace Cui;

//----------------------------------------------------------------

/* Draws something like this.

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

  This is drawn only once. The individual areas are each their own
  window class and later modify (and restore) their immediate boxing
  borders top/bottom/left/right as necessary with junctions to align
  with internal element field boundaries. And the NavBar window class
  will attach itself to the bottom by drawing and adjusting its outer
  border to overwrite "T's" instead of corners.

 ├                                                     ┤
 │                        NavBar                       │
 └─────────────────────────────────────────────────────┘
*/

void
LayoutCrapsScreen::draw()
{
    using S = Layout;
    using L = LayoutCrapsScreen;

    box(stdscr, 0, 0);  // 0,0 uses default chars vert and horiz char

#if 0    
    // Draw full screen outside borders. 4 lines.
    mvhline(S::screenBorderRowTop, S::screenBorderColLeft,  ACS_HLINE, S::screenWidth);
    mvhline(S::screenBorderRowBot, S::screenBorderColLeft,  ACS_HLINE, S::screenWidth);
    mvvline(S::screenBorderRowTop, S::screenBorderColLeft,  ACS_VLINE, S::screenHeight);
    mvvline(S::screenBorderRowTop, S::screenBorderColRight, ACS_VLINE, S::screenHeight);

    // Patch up outer four corners
    mvaddch(S::screenBorderRowTop, S::screenBorderColLeft,  ACS_ULCORNER);
    mvaddch(S::screenBorderRowTop, S::screenBorderColRight, ACS_URCORNER);
    mvaddch(S::screenBorderRowBot, S::screenBorderColLeft,  ACS_LLCORNER);
    mvaddch(S::screenBorderRowBot, S::screenBorderColRight, ACS_LRCORNER);
#endif
    
    // Internal Horizontal lines
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
    mvaddch(L::headerBorderTopRow, L::headerBorderRightCol,  ACS_TTEE);

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
}

//----------------------------------------------------------------
