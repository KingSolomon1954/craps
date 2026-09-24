//----------------------------------------------------------------
//
// File: WindowHouseBrief.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/PanelBase.h>
#include <cui/layouts/LayoutCrapsScreen.h>
#include <gen/MoneyUtils.h>

namespace Cui
{

    class WindowHouseBrief : PanelBase
{
public:
    /// @name Lifecycle
    /// @{
   ~WindowHouseBrief() = default;
    /// @}

    /// @name Modifiers
    /// @{
    void draw() override;
    void onTableResults(
        Gen::Money newBalance,           // how much left in bank
        int netBalance,                  // profit/loss session start
        unsigned numBetsTableWins,       // table wins session start, players lose
        unsigned numBetsTableLoses,      // table lose session start, players win
        Gen::Money tableIntakeLastRoll,  // table won last roll
        Gen::Money tableOutputLastRoll); // table lost last roll
    void onTableNumBetsOnTableChanged(
        unsigned numBetsOnTable,
        Gen::Money amtOnTable);
    /// @}

    /// @name Observers
    /// @{
    static WindowHouseBrief& instance();
    /// @}

private:
    struct Layout
    {
        using L = LayoutCrapsScreen;

        // Sizing and location based on LayoutCrapsScreen
        static constexpr int houseBriefTopRow   = L::houseBriefBorderTopRow   + 1;
        static constexpr int houseBriefBotRow   = L::houseBriefBorderBotRow   - 1;
        static constexpr int houseBriefLeftCol  = L::houseBriefBorderLeftCol  + 1;
        static constexpr int houseBriefRightCol = L::houseBriefBorderRightCol - 1;
        static constexpr int houseBriefHeight   = houseBriefBotRow   - houseBriefTopRow  + 1;
        static constexpr int houseBriefWidth    = houseBriefRightCol - houseBriefLeftCol + 1;
    };
    Gen::Money balance_             = 0;  // how much left in bank
    int netBalance_                 = 0;  // profit/loss session
    unsigned numBetsTableWins_      = 0;  // table wins session start, players lose
    unsigned numBetsTableLoses_     = 0;  // table lose session start, players win
    Gen::Money tableIntakeLastRoll_ = 0;  // table won last roll
    Gen::Money tableOutputLastRoll_ = 0;  // table lost last roll
    unsigned numBetsOnTable_ = 0;
    Gen::Money amtOnTable_ = 0;

private:
    WindowHouseBrief();
    void drawExternalJunctions();
    void drawInternalBorders();
    void drawStaticContent();
    void populate();
    void populateNetBalance();
    void populateNumBets();
    void populatePct();
    void populateOnTable();
    void populateLast();
    std::string tableWinPercentages() const;
};

} // namespace Cui

//----------------------------------------------------------------
