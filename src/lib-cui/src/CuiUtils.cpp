//---------------------------------------------------------------
//
// File: CuiUtils.cpp
//
//---------------------------------------------------------------

#include <cui/CuiUtils.h>  // Our interface

using namespace Cui;

/*-----------------------------------------------------------*//**

Move window contents to ncurses virtual backing store

@param[in] pWin
    The ncurses window of interest.

*/
void
CuiUtils::transfer(WINDOW* pWin)
{
    wnoutrefresh(pWin);
}

/*-----------------------------------------------------------*//**

Returns a ncurses WINDOW* sized with the given parameters.

@param[in] topRow
    The top row of of the window.

@param[in] botRow
    The bottom row of the window. 0 is the first row on the screen.
    Could be same row as the top row for a 1 line window. 

@param[in] lefCol
    The column of the start of the window.
    0 is the first colum on the screen.
    
@param[in] rightCol
    The column of the end of the window.

@return
    ncurses WINDOW* pointer
*/
WINDOW*
CuiUtils::newContentWindow(
    int topRow,
    int botRow,
    int leftCol,
    int rightCol)
{
    int height = botRow   - topRow  + 1;
    int width  = rightCol - leftCol + 1;
    int startY = topRow;
    int startX = leftCol;

    return newwin(height, width, startY, startX);
}

/*-----------------------------------------------------------*//**

Returns a ncurses WINDOW* sized with the given border parameters.

@param[in] borderTopRow
    The border row above the top of the window.
    0 is the first row on the screen.

@param[in] borderBotRow
    The border row below the bottom of the window.

@param[in] borderLeftCol
    The border column preceding the start of the window.
    0 is the first colum on the screen.
    
@param[in] borderRightCol
    The border column after the end of the window.

@return
    ncurses WINDOW* pointer
*/
WINDOW*
CuiUtils::newContentWindowByBorder(
    int borderTopRow,
    int borderBotRow,
    int borderLeftCol,
    int borderRightCol)

{
    int height = (borderBotRow   - 1) - (borderTopRow  - 1) + 1;
    int width  = (borderRightCol - 1) - (borderLeftCol - 1) + 1;
    int startY = borderTopRow  + 1;
    int startX = borderLeftCol + 1;

    return newwin(height, width, startY, startX);
}

//----------------------------------------------------------------
//
// Create a new window centered on the screen
// @param h Desired height
// @param w Desired width
// @return A new WINDOW* positioned at the center
//
WINDOW*
CuiUtils::makeCenteredWindow(int h, int w)
{
    int max_h, max_w;
    getmaxyx(stdscr, max_h, max_w);

    // Clamp requested size to screen size
    if (h > max_h) h = max_h;
    if (w > max_w) w = max_w;

    int start_y = (max_h - h) / 2;
    int start_x = (max_w - w) / 2;

    return newwin(h, w, start_y, start_x);
}

//----------------------------------------------------------------
