//----------------------------------------------------------------
//
// File: WindowPlayerBrief.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/PanelBase.h>
#include <cui/layouts/LayoutCrapsScreen.h>
#include <craps/CrapsTypes.h>
#include <gen/MoneyUtils.h>

namespace Cui
{

    class WindowPlayerBrief : PanelBase
{
public:
    /// @name Lifecycle
    /// @{
   ~WindowPlayerBrief() = default;
    /// @}

    /// @name Modifiers
    /// @{
    void draw() override;
    void onPlayerBalanceChanged(
        Craps::PlayerId playerId,
        Gen::Money      balance,         // how much left in wallet
        int             netBalance);     // profit/loss this session
    void onResolveBetsEnd();
    void onNewShooter(const Craps::PlayerId& playerId);
    /// @}

    /// @name Observers
    /// @{
    static WindowPlayerBrief& instance();
    /// @}

private:
    struct Layout
    {
        using L = LayoutCrapsScreen;

        // Sizing and location based on LayoutCrapsScreen
        static constexpr int playerBriefTopRow   = L::playerBriefBorderTopRow   + 1;
        static constexpr int playerBriefBotRow   = L::playerBriefBorderBotRow   - 1;
        static constexpr int playerBriefLeftCol  = L::playerBriefBorderLeftCol  + 1;
        static constexpr int playerBriefRightCol = L::playerBriefBorderRightCol - 1;
        static constexpr int playerBriefHeight   = playerBriefBotRow   - playerBriefTopRow  + 1;
        static constexpr int playerBriefWidth    = playerBriefRightCol - playerBriefLeftCol + 1;
    };
    
    Craps::PlayerId playerId_;
    std::string     playerName_;
    Gen::Money      balance_              = 0;  // how much left in wallet
    int             netBalance_           = 0;  // profit/loss this session
    unsigned        numBetsPlayerWins_    = 0;  // house wins session start, players lose
    unsigned        numBetsPlayerLoses_   = 0;  // house lose session start, players win
    Gen::Money      playerIntakeLastRoll_ = 0;  // house won last roll
    Gen::Money      playerOutputLastRoll_ = 0;  // house lost last roll
    unsigned        numBetsOnTable_       = 0;
    Gen::Money      amtOnTable_           = 0;
    std::string     shooter_;                   // empty to start

private:
    WindowPlayerBrief();
    void initPlayer();
    void drawExternalJunctions();
    void drawInternalBorders();
    void drawStaticContent();
    void populate();
    void populateName();
    void populateBalance();
    void populateNumBets();
    void populatePct();
    void populateOnTable();
    void populateLast();
    std::string playerWinPercentages() const;
};

} // namespace Cui

//----------------------------------------------------------------
