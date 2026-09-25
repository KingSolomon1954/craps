//----------------------------------------------------------------
//
// File: LayoutPlayerArea.h
//
//----------------------------------------------------------------

#pragma once

#include <array>

namespace Cui {
    
struct LayoutAllPlayers
{
    static constexpr int MaxPlayers = 6;
    
    // These are screen 0,0 coordinates, for external borders
    static constexpr int col1 = 0;
    static constexpr int col2 = 25;
    static constexpr int col3 = 48;
};

struct LayoutOnePlayer
{
    // These are screen 0,0 coordinates, for external borders
    static constexpr int col1 = 0;
    static constexpr int col2 = 19;
    static constexpr int col3 = 29;
    static constexpr int col4 = 39;
    static constexpr int col5 = 49;
    static constexpr int col6 = 59;
    static constexpr int colComeDont1 = 36;  // between come and dont come
        
    static constexpr int rowField    = 14;
    static constexpr int rowCraps    = 18;
    static constexpr int rowLineBets = 21;
};

} // namespace Cui

//----------------------------------------------------------------
