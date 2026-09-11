//---------------------------------------------------------------
//
// File: CuiUtils.cpp
//
//---------------------------------------------------------------

#include <cui/CuiUtils.h>

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

Returns a new ncurses WINDOW centered on the given pWin.

@param[in] pWin
    The window to be centered in

@param[in] h
    Height of the window to be centered

@param[in] w
    Width of  the window to be centered

@return
    ncurses WINDOW* pointer
*/
WINDOW*
CuiUtils::makeCenteredWindow(WINDOW* pWin, int h, int w)
{
    int max_h, max_w;
    getmaxyx(pWin, max_h, max_w);

    // Clamp requested size to pWin screen size
    if (h > max_h) h = max_h;
    if (w > max_w) w = max_w;

    int start_y = (max_h - h) / 2;
    int start_x = (max_w - w) / 2;

    return newwin(h, w, start_y, start_x);
}

//----------------------------------------------------------------

WindowRect
CuiUtils::getWindowRect(WINDOW* pWin)
{
    int h, w;
    getmaxyx(pWin, h, w);
    int r, c;
    getyx(pWin, h, w);
    
    return {r,c,h,w};
}

/*-----------------------------------------------------------*//**

Returns a WindowRect with coordinates centered on the given pWin.

@param[in] pWin
    The window to be centered in

@param[in] size
    The window size (rows, cols)

@return
    WindowRect with coordinates centered on pWin
*/
WindowRect
CuiUtils::centerRect(WINDOW* pWin, WindowSize size)
{
    WindowRect rect = getWindowRect(pWin);

    // Clamp requested size to pWin screen size
    if (size.rows > rect.rows) size.rows = rect.rows;
    if (size.cols > rect.cols) size.cols = rect.cols;
    
    int start_r = (size.rows - rect.rows) / 2;
    int start_c = (size.cols - rect.cols) / 2;
    
    return {start_r, start_c, size.rows, size.cols};
}

//----------------------------------------------------------------
