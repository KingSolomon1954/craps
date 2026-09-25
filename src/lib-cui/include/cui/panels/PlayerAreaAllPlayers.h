//----------------------------------------------------------------
//
// File: PlayerAreaAllPlayers.h
//
// Manages the AllPlayers view within the WindowPlayerArea class.
//
//----------------------------------------------------------------

#pragma once

#include <cui/layouts/LayoutPlayerArea.h>
#include <ncurses.h>
#include <array>
#include <string_view>
#include <vector>

namespace Cui
{

class PlayerAreaAllPlayers
{
public:
    /// @name Lifecycle
    /// @{
    PlayerAreaAllPlayers(int winHeight, int winWidth);
   ~PlayerAreaAllPlayers() = default;
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
    enum class BetState
    {
        None,
        Bet,
        BetWithOdds
    };

    struct Player
    {
        wchar_t initial = L'\u254C'; // ┌─ placeholder: ─
        short colorPair = 0;
    };

    struct Bet
    {
        std::string_view label;
        int row;
        int section;
        std::array<BetState, LayoutAllPlayers::MaxPlayers> state{};
    };

    struct Layout
    {
        // These are WINDOW 0,0 coordinates inside of border
        static constexpr int col1_2 = 24;  // vertical line between line bets and place bets
        static constexpr int col2_3 = 47;  // vertical line between place bets and prop bets

        static constexpr wchar_t FilledCircle = L'\u25CF';  // ● filled circle 
        static constexpr wchar_t HollowCircle = L'\u25CB';  // ○ hollow circle 
        static constexpr wchar_t Dot          = L'\u22C5';  // '⋅'
        static constexpr wchar_t DoubleDash   = L'\u254C';  // '╌'

        static constexpr int HeaderRow = 1;
        static constexpr int FirstBetRow = 2;

        // Interior left edges of the three sections.
        static constexpr std::array<int, 3> SectionX = {
            1,   // first section
            26,  // second section
            48   // third section
        };

        // Label begins two columns after each section's left border.
        static constexpr int LabelOffset = 1;

        // First player marker begins after the fixed-width label.
        static constexpr int PlayerOffset = 12;

        // Each player column is two terminal columns apart.
        static constexpr int PlayerStride = 2;
    };

private:
    WINDOW* pWin_  = nullptr;
    int winHeight_ = 0;
    int winWidth_  = 0;
    std::vector<Player> players_;
    std::vector<Bet> bets_;
    
};

} // namespace Cui

//----------------------------------------------------------------
