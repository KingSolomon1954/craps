//----------------------------------------------------------------
//
// File: PlayerAreaOnePlayer.h
//
// Manages the OnePlayer view within the WindowPlayerArea class.
//
//----------------------------------------------------------------

#pragma once

#include <craps/CrapsTypes.h>
#include <craps/EnumBetName.h>
#include <cui/CuiUtils.h>
#include <gen/MoneyUtils.h>
#include <ncurses.h>
#include <vector>

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
    void init(WINDOW* pWin);
    void drawInternalBorders();
    void drawStaticContent();
    void advancePlayer(bool next);

    void onBetMade(
        const Craps::PlayerId& playerId,
        Craps::BetId           betId,
        BetName                betName,
        Gen::Money             contractAmount,
        Gen::Money             oddsAmount,
        unsigned               pivot);

    void onBetResolved();

    void onPlayerJoinedTable(const Craps::PlayerId& playerId);
    void onPlayerLeftTable  (const Craps::PlayerId& playerId);
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

        static constexpr int betLabelWidth  = 8;
        static constexpr int betAmountWidth = 8;
    };

    struct Player
    {
        Craps::PlayerId playerId;
        std::string name;
        short colorPair;
    };

    struct BetLabel
    {
        std::string_view label;
        int row;
        int col;
    };

    std::array<BetLabel, 27> labels_{
        "4",         0, 13,
        "5",         0, 23,
        "6",         0, 33,
        "8",         0, 43,
        "9",         0, 53,
        "10",        0, 63,
        "Place",     1, 1,
        "Come",      2, 1,
        "Odds",      3, 1,
        "DontCome",  4, 1,
        "Odds",      5, 1,
        "Buy",       6, 1,
        "Lay",       7, 1,
        "Hardways",  8, 1,
        "Field",    10, 32,
        "2  3  4  9  10  11  12", 11, 24,
        "AnyCraps", 14, 1,
        "C&E",      14, 29,
        "Horn",     14, 54,
        "Any7",     15, 1,
        "World",    15, 54,
        "Come",     17, 1,
        "DontCome", 17, 37,
        "PassLine", 18, 1,
        "Odds",     18, 20,
        "DontPass", 18, 37,
        "Odds",     18, 55
    };

    struct Bet
    {
        CuiUtils::CuiBetName betName;
        int row;
        int col;
        Gen::Money amount = 0;
    };

    std::array<Bet, 58> bets_{
        CuiUtils::CuiBetName::Place4,        1, 10, 0,
        CuiUtils::CuiBetName::Place5,        1, 20, 0,
        CuiUtils::CuiBetName::Place6,        1, 30, 0,
        CuiUtils::CuiBetName::Place8,        1, 40, 0,
        CuiUtils::CuiBetName::Place9,        1, 50, 0,
        CuiUtils::CuiBetName::Place10,       1, 60, 0,
        CuiUtils::CuiBetName::Come4,         2, 10, 0,
        CuiUtils::CuiBetName::Come5,         2, 20, 0,
        CuiUtils::CuiBetName::Come6,         2, 30, 0,
        CuiUtils::CuiBetName::Come8,         2, 40, 0,
        CuiUtils::CuiBetName::Come9,         2, 50, 0,
        CuiUtils::CuiBetName::Come10,        2, 60, 0,
        CuiUtils::CuiBetName::ComeOdds4,     3, 10, 0,
        CuiUtils::CuiBetName::ComeOdds5,     3, 20, 0,
        CuiUtils::CuiBetName::ComeOdds6,     3, 30, 0,
        CuiUtils::CuiBetName::ComeOdds8,     3, 40, 0,
        CuiUtils::CuiBetName::ComeOdds9,     3, 50, 0,
        CuiUtils::CuiBetName::ComeOdds10,    3, 60, 0,
        CuiUtils::CuiBetName::DontCome4,     4, 10, 0,
        CuiUtils::CuiBetName::DontCome5,     4, 20, 0,
        CuiUtils::CuiBetName::DontCome6,     4, 30, 0,
        CuiUtils::CuiBetName::DontCome8,     4, 40, 0,
        CuiUtils::CuiBetName::DontCome9,     4, 50, 0,
        CuiUtils::CuiBetName::DontCome10,    4, 60, 0,
        CuiUtils::CuiBetName::DontOdds4,     5, 10, 0,
        CuiUtils::CuiBetName::DontOdds5,     5, 20, 0,
        CuiUtils::CuiBetName::DontOdds6,     5, 30, 0,
        CuiUtils::CuiBetName::DontOdds8,     5, 40, 0,
        CuiUtils::CuiBetName::DontOdds9,     5, 50, 0,
        CuiUtils::CuiBetName::DontOdds10,    5, 60, 0,
        CuiUtils::CuiBetName::Buy4,          6, 10, 0,
        CuiUtils::CuiBetName::Buy5,          6, 20, 0,
        CuiUtils::CuiBetName::Buy6,          6, 30, 0,
        CuiUtils::CuiBetName::Buy8,          6, 40, 0,
        CuiUtils::CuiBetName::Buy9,          6, 50, 0,
        CuiUtils::CuiBetName::Buy10,         6, 60, 0,
        CuiUtils::CuiBetName::Lay4,          7, 10, 0,
        CuiUtils::CuiBetName::Lay5,          7, 20, 0,
        CuiUtils::CuiBetName::Lay6,          7, 30, 0,
        CuiUtils::CuiBetName::Lay8,          7, 40, 0,
        CuiUtils::CuiBetName::Lay9,          7, 50, 0,
        CuiUtils::CuiBetName::Lay10,         7, 60, 0,
        CuiUtils::CuiBetName::Hard4,         8, 10, 0,
        CuiUtils::CuiBetName::Hard6,         8, 30, 0,
        CuiUtils::CuiBetName::Hard8,         8, 40, 0,
        CuiUtils::CuiBetName::Hard10,        8, 60, 0,
        CuiUtils::CuiBetName::Field,        12, 31, 0,
        CuiUtils::CuiBetName::AnyCraps,     14, 10, 0,
        CuiUtils::CuiBetName::CandE,        14, 33, 0,
        CuiUtils::CuiBetName::Horn,         14, 60, 0,
        CuiUtils::CuiBetName::Any7,         15, 10, 0,
        CuiUtils::CuiBetName::World,        15, 60, 0,
        CuiUtils::CuiBetName::Come,         17, 10, 0,
        CuiUtils::CuiBetName::DontCome,     17, 46, 0,
        CuiUtils::CuiBetName::PassLine,     18, 10, 0,
        CuiUtils::CuiBetName::PassLineOdds, 18, 25, 0,
        CuiUtils::CuiBetName::DontPass,     18, 46, 0,
        CuiUtils::CuiBetName::DontPassOdds, 18, 60, 0
    };

private:
    WINDOW* pWin_    = nullptr;
    int winHeight_   = 0;
    int winWidth_    = 0;
    int playerIndex_ = 0;
    std::vector<Player> players_;

private:
    void buildPlayerInfo();
    void buildBetInfo();
    std::size_t removePlayer(const Craps::PlayerId& id);
    void drawName();
    void drawLabels();
    void drawBets();
    void drawOneBet(const Bet& b);
    int getBetIndex(BetName betName, unsigned pivot) const;
    void refreshAllBets();
    void clearBets();
    BetName getBetName(Craps::BetId betId) const;
    unsigned getPivot(Craps::BetId betId) const;
    Gen::Money getContractAmount(Craps::BetId betId) const;
    Gen::Money getOddsAmount(Craps::BetId betId) const;
    void nextPlayerIndex();
    void prevPlayerIndex();
};

} // namespace Cui

//----------------------------------------------------------------
