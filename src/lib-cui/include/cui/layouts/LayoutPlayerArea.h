//----------------------------------------------------------------
//
// File: LayoutPlayerArea.h
//
//----------------------------------------------------------------

#pragma once

namespace Cui {
    
struct LayoutAllPlayers
{
    static constexpr int col1 = 0;
    static constexpr int col2 = 25;
    static constexpr int col3 = 48;
};

struct LayoutOnePlayer
{
    static constexpr int col1 = 0;
    static constexpr int col2 = 19;
    static constexpr int col3 = 29;
    static constexpr int col4 = 39;
    static constexpr int col5 = 49;
    static constexpr int col6 = 59;
    static constexpr int lineBetSplitCol = 59;

    static constexpr int rowField    = 14;
    static constexpr int rowCraps    = 18;
    static constexpr int rowLineBets = 21;
};

} // namespace Cui

//----------------------------------------------------------------
