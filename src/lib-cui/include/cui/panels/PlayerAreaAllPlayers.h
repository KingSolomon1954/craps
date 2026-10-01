//----------------------------------------------------------------
//
// File: PlayerAreaAllPlayers.h
//
// Manages the AllPlayers view within the WindowPlayerArea class.
//
//----------------------------------------------------------------

#pragma once

#include <cui/layouts/LayoutPlayerArea.h>
#include <cui/CuiStructs.h>
#include <cui/CuiUtils.h>
#include <craps/CrapsTypes.h>
#include <craps/EnumBetName.h>
#include <gen/MoneyUtils.h>
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
    void init(WINDOW* pWin);
    void drawInternalBorders();
    void drawStaticContent();

    void onBetMade(const Craps::PlayerId& playerId,
                         Craps::BetId     betId,
                         BetName          betName,
                         Gen::Money       contractAmount,
                         Gen::Money       oddsAmount,
                         unsigned         pivot);

    void onBetResolved(const Craps::PlayerId& playerId,
                       BetName                betName,
                       Craps::BetId           betId,
                       Gen::Money             amountWin,
                       Gen::Money             amountLose);


    void onPlayerJoinedTable(const Craps::PlayerId& playerId);
    void onPlayerLeftTable  (const Craps::PlayerId& playerId);
    /// @}

    /// @name Observers
    /// @{
    /// @}

private:

    struct BetLabel
    {
        std::string_view label;
        int row;
        int col;
    };

    std::array<BetLabel, 44> labels_{
        "PassLine",    2, 1,
        "Come",        3, 1,
        "Come4",       4, 1,
        "Come5",       5, 1,
        "Come6",       6, 1,
        "Come8",       7, 1,
        "Come9",       8, 1,
        "Come10",      9, 1,
        "DontPass",   10, 1,
        "DontCome",   11, 1,
        "DontCome4",  12, 1,
        "DontCome5",  13, 1,
        "DontCome6",  14, 1,
        "DontCome8",  15, 1,
        "DontCome9",  16, 1,
        "DontCome10", 17, 1,
            
        "Place4",      2, 26,
        "Place5",      3, 26,
        "Place6",      4, 26,
        "Place8",      5, 26,
        "Place9",      6, 26,
        "Place10",     7, 26,
        "Hard4",       8, 26,
        "Hard6",       9, 26,
        "Hard8",      10, 26,
        "Hard10",     11, 26,
        "C&E",        12, 26,
        "Field",      13, 26,
            
        "Buy4",        2, 49,
        "Buy5",        3, 49,
        "Buy6",        4, 49,
        "Buy8",        5, 49,
        "Buy9",        6, 49,
        "Buy10",       7, 49,
        "Lay4",        8, 49,
        "Lay5",        9, 49,
        "Lay6",       10, 49,
        "Lay8",       11, 49,
        "Lay9",       12, 49,
        "Lay10",      13, 49,
        "AnyC",       14, 49,
        "Any7",       15, 49,
        "Horn",       16, 49,
        "World",      17, 49
    };

    enum class BetState
    {
        None,
        Bet,
        BetWithOdds
    };

    struct Player
    {
        Craps::PlayerId playerId;
        wchar_t initial = L'\u254C'; // ┌─ placeholder: ─
        short colorPair = 0;
    };

    struct Detail
    {
        Craps::BetId betId = 0;
        BetState     state = BetState::None;
        int col;
    };
    
    struct Bet
    {
        CuiUtils::CuiBetName betName;
        int row;
        std::array<Detail, MaxPlayers> detail{};
    };

    static constexpr std::array<Detail, MaxPlayers> detailSection1()
    {
        return {{
            {0, BetState::None, 12},
            {0, BetState::None, 14},
            {0, BetState::None, 16},
            {0, BetState::None, 18},
            {0, BetState::None, 20},
            {0, BetState::None, 22}
        }};
    }
    
    static constexpr std::array<Detail, MaxPlayers> detailSection2()
    {
        return {{
            {0, BetState::None, 35},
            {0, BetState::None, 37},
            {0, BetState::None, 39},
            {0, BetState::None, 41},
            {0, BetState::None, 43},
            {0, BetState::None, 45}
        }};
    }
    
    static constexpr std::array<Detail, MaxPlayers> detailSection3()
    {
        return {{
            {0, BetState::None, 56},
            {0, BetState::None, 58},
            {0, BetState::None, 60},
            {0, BetState::None, 62},
            {0, BetState::None, 64},
            {0, BetState::None, 66}
        }};
    }
    
    std::array<Bet, 44> bets_
    {{
        { CuiUtils::CuiBetName::PassLine,    2, detailSection1() },
        { CuiUtils::CuiBetName::Come,        3, detailSection1() },
        { CuiUtils::CuiBetName::Come4,       4, detailSection1() },
        { CuiUtils::CuiBetName::Come5,       5, detailSection1() },
        { CuiUtils::CuiBetName::Come6,       6, detailSection1() },
        { CuiUtils::CuiBetName::Come8,       7, detailSection1() },
        { CuiUtils::CuiBetName::Come9,       8, detailSection1() },
        { CuiUtils::CuiBetName::Come10,      9, detailSection1() },
        { CuiUtils::CuiBetName::DontPass,   10, detailSection1() },
        { CuiUtils::CuiBetName::DontCome,   11, detailSection1() },
        { CuiUtils::CuiBetName::DontCome4,  12, detailSection1() },
        { CuiUtils::CuiBetName::DontCome5,  13, detailSection1() },
        { CuiUtils::CuiBetName::DontCome6,  14, detailSection1() },
        { CuiUtils::CuiBetName::DontCome8,  15, detailSection1() },
        { CuiUtils::CuiBetName::DontCome9,  16, detailSection1() },
        { CuiUtils::CuiBetName::DontCome10, 17, detailSection1() },

        { CuiUtils::CuiBetName::Place4,      2, detailSection2() },
        { CuiUtils::CuiBetName::Place5,      3, detailSection2() },
        { CuiUtils::CuiBetName::Place6,      4, detailSection2() },
        { CuiUtils::CuiBetName::Place8,      5, detailSection2() },
        { CuiUtils::CuiBetName::Place9,      6, detailSection2() },
        { CuiUtils::CuiBetName::Place10,     7, detailSection2() },
        { CuiUtils::CuiBetName::Hard4,       8, detailSection2() },
        { CuiUtils::CuiBetName::Hard6,       9, detailSection2() },
        { CuiUtils::CuiBetName::Hard8,      10, detailSection2() },
        { CuiUtils::CuiBetName::Hard10,     11, detailSection2() },
        { CuiUtils::CuiBetName::CandE,      12, detailSection2() },
        { CuiUtils::CuiBetName::Field,      13, detailSection2() },

        { CuiUtils::CuiBetName::Buy4,        2, detailSection3() },
        { CuiUtils::CuiBetName::Buy5,        3, detailSection3() },
        { CuiUtils::CuiBetName::Buy6,        4, detailSection3() },
        { CuiUtils::CuiBetName::Buy8,        5, detailSection3() },
        { CuiUtils::CuiBetName::Buy9,        6, detailSection3() },
        { CuiUtils::CuiBetName::Buy10,       7, detailSection3() },
        { CuiUtils::CuiBetName::Lay4,        8, detailSection3() },
        { CuiUtils::CuiBetName::Lay5,        9, detailSection3() },
        { CuiUtils::CuiBetName::Lay6,       10, detailSection3() },
        { CuiUtils::CuiBetName::Lay8,       11, detailSection3() },
        { CuiUtils::CuiBetName::Lay9,       12, detailSection3() },
        { CuiUtils::CuiBetName::Lay10,      13, detailSection3() },
        { CuiUtils::CuiBetName::AnyCraps,   14, detailSection3() },
        { CuiUtils::CuiBetName::Any7,       15, detailSection3() },
        { CuiUtils::CuiBetName::Horn,       16, detailSection3() },
        { CuiUtils::CuiBetName::World,      17, detailSection3() }
    }};

    struct OldBet
    {
        std::string_view label;
        int row;
        int section;
        std::array<BetState, MaxPlayers> state{};
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

        static constexpr int HeaderRow   = 1;
        static constexpr int FirstBetRow = 2;

        // Interior left edges of the three sections.
        static constexpr std::array<int, 3> SectionX =
        {
            0,   // first section
            25,  // second section
            48   // third section
        };

        // Label width of each section.
        static constexpr std::array<int, 3> SectionW =
        {
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

private:
    void buildPlayerInfo();
    std::size_t removePlayer(const Craps::PlayerId& playerId);

    void drawPlayerHeaders();
    void drawSectionHeader(int startCol);
    void drawLabels();
    void drawBetMarkers();
    void drawBetMarker(int row, int col, BetState state, short colorPair);
    void drawWideCharacter(int row, int col,
                           wchar_t ch, short colorPair);
    wchar_t chooseInitial(std::string_view name) const;
    std::size_t getPlayerIndex(const Craps::PlayerId& playerId) const;
    std::size_t getBetIndex(CuiUtils::CuiBetName betName,
                            std::size_t playerIndex) const;
    BetState calcBetState(Gen::Money contractAmount,
                          Gen::Money oddsAmount) const;
    wchar_t getMarker(BetState s) const;
};

} // namespace Cui

//----------------------------------------------------------------
