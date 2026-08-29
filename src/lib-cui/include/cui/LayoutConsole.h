//----------------------------------------------------------------
//
// File: LayoutConsole.h
//
//----------------------------------------------------------------

#pragma once

namespace Cui {

struct LayoutConsole
{
    // Minimum supported terminal size 41x100
    static constexpr int minRows = 41;
    static constexpr int minCols = 100;

    // Content area occupies rows 0-37, cols 0-99
    static constexpr int contentTopRow   = 0;
    static constexpr int contentBotRow   = 37;
    static constexpr int contentLeftCol  = 0;
    static constexpr int contentRightCol = 99;
    static constexpr int contentHeight   = contentBotRow   - contentTopRow  + 1;
    static constexpr int contentWidth    = contentRightCol - contentLeftCol + 1;

    // Nav Bar occupies rows 38-40, cols 0-99
    static constexpr int navBarBorderTopRow   = 38;
    static constexpr int navBarBorderBotRow   = 40;
    static constexpr int navBarBorderLeftCol  = 0;
    static constexpr int navBarBorderRightCol = 99;
    static constexpr int navBarBorderHeight   = navBarBorderBotRow   - navBarBorderTopRow  + 1;
    static constexpr int navBarBorderWidth    = navBarBorderRightCol - navBarBorderLeftCol + 1;
};

} // namespace Cui

//----------------------------------------------------------------
