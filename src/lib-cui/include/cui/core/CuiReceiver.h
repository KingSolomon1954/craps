//----------------------------------------------------------------
//
// File: CuiReceiver.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/GameEvents.h>

namespace Cui {

class CuiReceiver
{
public:
    /// @name Lifecycle
    /// @{
    CuiReceiver();
   ~CuiReceiver() = default;
    static CuiReceiver& instance();
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}

private:
    void onRspMakeBet                (const Ctrl::RspMakeBet& ev)                   const;
    void onRspOddsBet                (const Ctrl::RspOddsBet& ev)                   const;
    void onRspSwitchPlayer           (const Ctrl::RspSwitchPlayer& ev)              const;
    void onRspManagePlayer           (const Ctrl::RspManagePlayer& ev)              const;
    void onBettingClosed             (const Ctrl::UslBettingClosed& ev)             const;
    void onDiceNewValue              (const Ctrl::UslDiceNewValue& ev)              const;
    void onDiceThrowStart            (const Ctrl::UslDiceThrowStart& ev)            const;
    void onResolveBetsEnd            (const Ctrl::UslResolveBetsEnd& ev)            const;
    void onPointEstablished          (const Ctrl::UslPointEstablished& ev)          const;
    void onSevenOut                  (const Ctrl::UslSevenOut& ev)                  const;
    void onPassLineWinner            (const Ctrl::UslPassLineWinner& ev)            const;
    void onNewShooter                (const Ctrl::UslNewShooter& ev)                const;
    void onBettingOpened             (const Ctrl::UslBettingOpened& ev)             const;
    void onBetMade                   (const Ctrl::UslBetMade& ev)                   const;
    void onRspRemoveBet              (const Ctrl::RspRemoveBet& ev)                 const;
    void onRspUndoBet                (const Ctrl::RspUndoBet& ev)                   const;
    void onRspApplyQuickBets         (const Ctrl::RspApplyQuickBets& ev)            const;
    void onRspEditQuickBets          (const Ctrl::RspEditQuickBets& ev)             const;
    void onRspEditAutoOdds           (const Ctrl::RspEditAutoOdds& ev)              const;
    void onAutoOddsApplied           (const Ctrl::UslAutoOddsApplied& ev)           const;
    void onBetRemoved                (const Ctrl::UslBetRemoved& ev)                const;
    void onRollHistoryFormatChanged  (const Ctrl::UslRollHistoryFormatChanged& ev) const;
    void onBetChanged                (const Ctrl::UslBetChanged& ev)                const;
    void onBetResolved               (const Ctrl::UslBetResolved& ev)               const;

    void onTableBetsChanged          (const Ctrl::UslTableBetsChanged& ev)          const;
    void onTableLastRollStatsChanged (const Ctrl::UslTableLastRollStatsChanged& ev) const;
    void onTableSessionStatsChanged  (const Ctrl::UslTableSessionStatsChanged& ev)  const;
    void onTableBalanceChanged       (const Ctrl::UslTableBalanceChanged& ev)       const;

    void onPlayerBetsChanged         (const Ctrl::UslPlayerBetsChanged& ev)          const;
    void onPlayerLastRollStatsChanged(const Ctrl::UslPlayerLastRollStatsChanged& ev) const;
    void onPlayerSessionStatsChanged (const Ctrl::UslPlayerSessionStatsChanged& ev)  const;
    void onPlayerBalanceChanged      (const Ctrl::UslPlayerBalanceChanged& ev)      const;
    void onPlayerJoinedTable         (const Ctrl::UslPlayerJoinedTable& ev)         const;
    void onPlayerLeftTable           (const Ctrl::UslPlayerLeftTable& ev)           const;
    void onUserPlayerSwitched        (const Ctrl::UslUserPlayerSwitched& ev)        const;
    void onPlayerRenamed             (const Ctrl::UslPlayerRenamed& ev)             const;
};

/*-----------------------------------------------------------*//**

@class CuiReceiver

@brief Receives application GameEvents

Receives application GameEvents and converts them
into work orders for the CuiThread.

Responsibilities of CuiReceiver:

@li Event handler for GameEvents the CUI is interested in
@li Is the interface between the Controller's view vocabulary and the
    CUI's surface vocabulary.
@li Subscribes to Controller::EventManager
@li Converts events into work orders and enqueues the work order to
    the CuiThread for processing.
*/

} // namespace Cui

//----------------------------------------------------------------
