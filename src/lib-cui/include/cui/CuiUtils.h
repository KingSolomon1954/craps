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
    /// @name Lifecycle
    /// @{
    /// @}

    /// @name Modifiers
    /// @{
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
    /// @}

    /// @name Observers
    /// @{
    /// @}
};


/*-----------------------------------------------------------*//**

@class CuiUtils

@brief Some useful static functions for working with ncurses.

*/

//----------------------------------------------------------------

}  // namespace Cui
