//----------------------------------------------------------------
//
// File: LayoutConsole.h
//
//----------------------------------------------------------------

#pragma once

#include <cassert>

namespace Cui {

// Layout shared across all full screens

struct LayoutConsole
{
    // Minimum supported terminal size 41x100
    static constexpr int minRows = 41;
    static constexpr int minCols = 100;

    // Full screen windows 41x100
    static constexpr int topRow   = 0;
    static constexpr int botRow   = minRows - 1;
    static constexpr int leftCol  = 0;
    static constexpr int rightCol = minCols - 1;
    static constexpr int height   = minRows;
    static constexpr int width    = minCols;

    static_assert(topRow   == 0);
    static_assert(botRow   == 40);
    static_assert(leftCol  == 0);
    static_assert(rightCol == 99);
    static_assert(height   == 41);
    static_assert(width    == 100);

    // Nav Bar shared across all full screens
    // Nav Bar occupies rows 38-40, cols 0-99, at bottom of full screen
    static constexpr int navBorderTopRow   = 38;
    static constexpr int navBorderBotRow   = botRow;
    static constexpr int navBorderLeftCol  = leftCol;
    static constexpr int navBorderRightCol = rightCol;
    static constexpr int navBorderHeight   = navBorderBotRow   - navBorderTopRow  + 1;
    static constexpr int navBorderWidth    = navBorderRightCol - navBorderLeftCol + 1;

    static_assert(navBorderTopRow   == 38);
    static_assert(navBorderBotRow   == 40);
    static_assert(navBorderLeftCol  == 0);
    static_assert(navBorderRightCol == 99);
    static_assert(navBorderHeight   == 3);
    static_assert(navBorderWidth    == 100);
};

} // namespace Cui

//----------------------------------------------------------------
