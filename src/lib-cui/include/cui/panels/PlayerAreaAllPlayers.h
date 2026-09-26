//----------------------------------------------------------------
//
// File: PlayerAreaAllPlayers.h
//
// Manages the AllPlayers view within the WindowPlayerArea class.
//
//----------------------------------------------------------------

#pragma once

#include <cui/layouts/LayoutPlayerArea.h>
#include <craps/CrapsTypes.h>
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
    void init(WINDOW* pWin, const std::vector<Craps::PlayerId>& playerIds);
    void drawInternalBorders();
    void drawStaticContent();
    void onBetMade();
    void onPlayerJoinedTable();
    void onPlayerLeftTable();
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
            0,   // first section
            25,  // second section
            48   // third section
        };

        // Label width of each section.
        static constexpr std::array<int, 3> SectionW = {
            12,   // first section
            9,    // second section
            7     // third section
        };

        // Each player column is two terminal columns apart.
        static constexpr int PlayerStride = 2;
    };

private:
    WINDOW* pWin_  = nullptr;
    int winHeight_ = 0;
    int winWidth_  = 0;
    std::vector<Player> players_;
    std::vector<Bet> bets_;

private:
    void buildBetInfo();
    void buildPlayerInfo(const std::vector<Craps::PlayerId>& playerIds);
    void drawPlayerHeaders();
    void drawBetLabels();
    void drawBetMarkers();
    void drawWideCharacter(int row, int col,
                           wchar_t ch, short colorPair);
    void setBetState(std::size_t betIndex,
                     std::size_t playerIndex,
                     BetState state);

    void setBetState(std::string_view betName,
                     std::size_t playerIndex,
                     BetState state);

    bool betPosition(std::size_t betIndex,
                     std::size_t playerIndex,
                     int& row,
                     int& col) const;
    wchar_t chooseInitial(std::string_view name) const;
    short playerColorPair(std::size_t index) const;
};

} // namespace Cui

//----------------------------------------------------------------
