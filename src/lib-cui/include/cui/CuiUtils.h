//----------------------------------------------------------------
//
// File: CuiUtils.h
//
//----------------------------------------------------------------

#pragma once

#include <ncurses.h>

//----------------------------------------------------------------

namespace Cui {

class CuiUtils
{
public:
    /// @name CUI Utilities
    /// @{
    static void transfer(WINDOW* pWin);
    static WINDOW* newContentWindow(
        int topRow,
        int botRow,
        int leftCol,
        int rightCol);
    static WINDOW* newContentWindowByBorder(
        int borderTopRow,
        int borderBotRow,
        int borderLeftCol,
        int borderRightCol);
    static WINDOW* makeCenteredWindow(
        int h,
        int w);
    /// @}
};

/*-----------------------------------------------------------*//**

@class CuiUtils

@brief Some useful static functions for working with ncurses.

*/

//----------------------------------------------------------------

}  // namespace Cui
