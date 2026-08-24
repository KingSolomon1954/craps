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
    static constexpr int contentRowTop   = 0;
    static constexpr int contentRowBot   = 37;
    static constexpr int contentColLeft  = 0;
    static constexpr int contentColRight = 99;
    static constexpr int contentHeight   = contentRowBot   - contentRowTop  + 1;
    static constexpr int contentWidth    = contentColRight - contentColLeft + 1;
};

} // namespace Cui

//----------------------------------------------------------------
