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
    void onHouseResults(
        int newBalance,                  // from session start, starting at 0
        unsigned numBetsHouseWins,       // house wins session start, players lose
        unsigned numBetsHouseLoses,      // house lose session start, players win
        Gen::Money houseIntakeLastRoll,  // house won last roll
        Gen::Money houseOutputLastRoll); // house lost last roll
    void onNumBetsOnTableChanged(
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
    int balance_                    = 0;  // since session start from 0
    unsigned numBetsHouseWins_      = 0;  // house wins session start, players lose
    unsigned numBetsHouseLoses_     = 0;  // house lose session start, players win
    Gen::Money houseIntakeLastRoll_ = 0;  // house won last roll
    Gen::Money houseOutputLastRoll_ = 0;  // house lost last roll
    unsigned numBetsOnTable_ = 0;
    Gen::Money amtOnTable_ = 0;

private:
    WindowHouseBrief();
    void drawExternalJunctions();
    void drawInternalBorders();
    void drawStaticContent();
    void populate();
    void populateBalance();
    void populateNumBets();
    void populatePct();
    void populateOnTable();
    void populateLast();
    std::string houseWinPercentages() const;
};

} // namespace Cui

//----------------------------------------------------------------
