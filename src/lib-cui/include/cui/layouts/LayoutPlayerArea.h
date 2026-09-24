//----------------------------------------------------------------
//
// File: LayoutPlayerArea.h
//
//----------------------------------------------------------------

#pragma once

namespace Cui {
    
struct LayoutAllPlayers
{
    // These are screen 0,0 coordinates, for external borders
    static constexpr int col1 = 0;
    static constexpr int col2 = 25;
    static constexpr int col3 = 48;

    // These are WINDOW 0,0 coordinates inside of border
    static constexpr int col1_2 = 24;  // vertical line between line bets and place bets
    static constexpr int col2_3 = 47;  // vertical line between place bets and prop bets
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

    // These are WINDOW 0,0 coordinates inside of border
    static constexpr int col4_5  = 18;     // vertical line between numbers
    static constexpr int col5_6  = 28;
    static constexpr int col6_8  = 38;
    static constexpr int col8_9  = 48;
    static constexpr int col9_10 = 58;
    static constexpr int colComeDont2 = 35;  // between come and dont come

    static constexpr int fieldBorderTopRow    = 9;
    static constexpr int crapsBorderTopRow    = 13;
    static constexpr int lineBetsBorderTopRow = 16;
    
    static constexpr int rowsNumbers  = 9;
    static constexpr int rowsField    = 3;
    static constexpr int rowsCraps    = 2;
    static constexpr int rowsLineBets = 2;
};

} // namespace Cui

//----------------------------------------------------------------
