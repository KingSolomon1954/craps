//----------------------------------------------------------------
//
// File: PlayerAreaOnePlayer.h
//
// Manages the OnePlayer view within the WindowPlayerArea class.
//
//----------------------------------------------------------------

#pragma once

#include <ncurses.h>

namespace Cui
{

class PlayerAreaOnePlayer
{
public:
    /// @name Lifecycle
    /// @{
    PlayerAreaOnePlayer(int winHeight, int winWidth);
   ~PlayerAreaOnePlayer() = default;
    /// @}

    /// @name Modifiers
    /// @{
     void setWindow(WINDOW* pWin);
     void drawInternalBorders();
    /// @}

    /// @name Observers
    /// @{
    /// @}

private:
    struct Layout
    {
        static constexpr int rowsNumbers  = 9;
        static constexpr int rowsField    = 3;
        static constexpr int rowsCraps    = 2;
        static constexpr int rowsLineBets = 2;

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
    };
    
private:
    WINDOW* pWin_  = nullptr;
    int winHeight_ = 0;
    int winWidth_  = 0;
   
};

} // namespace Cui

//----------------------------------------------------------------
