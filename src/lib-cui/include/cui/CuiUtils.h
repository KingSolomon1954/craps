//----------------------------------------------------------------
//
// File: CuiUtils.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/CuiStructs.h>
#include <ncurses.h>

//----------------------------------------------------------------

namespace Cui {

class CuiUtils
{
public:
    /// @name CUI Utilities
    /// @{
    static void transfer(WINDOW* pWin);
    static WINDOW* makeCenteredWindow(WINDOW* pWin, int h, int w);
    static WindowRect centerRect(WINDOW* pWin, WindowSize size);
    static WindowRect getWindowRect(WINDOW* pWin);
    /// @}
};

/*-----------------------------------------------------------*//**

@class CuiUtils

@brief Some useful static functions for working with ncurses.

*/

//----------------------------------------------------------------

}  // namespace Cui
