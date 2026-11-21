//----------------------------------------------------------------
//
// File: WorkOrderDispatcher.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/core/WorkOrder.h>
#include <chrono>
#include <functional>
#include <vector>

//----------------------------------------------------------------

namespace Cui {

class WorkOrderDispatcher
{
public:
    WorkOrderDispatcher() = default;
   ~WorkOrderDispatcher() = default;

    WorkOrderDispatcher(const WorkOrderDispatcher&)            = delete;
    WorkOrderDispatcher& operator=(const WorkOrderDispatcher&) = delete;
    WorkOrderDispatcher(WorkOrderDispatcher&&)                 = delete;
    WorkOrderDispatcher& operator=(WorkOrderDispatcher&&)      = delete;
    
    void dispatch(const WorkOrder& wo);  // Main entry point

    // Called regularly by the CuiThread. Safety net that releases held
    // results if the animation never reports that the dice have landed.
    void tick();
    void releaseHeldEvents();
    
private:
    void process(const WorkOrderKey& wo);
    void process(const WorkOrderSurface& wo);
    void process(const WorkOrderEvent& wo);
    
    void process(const Ctrl::UslBettingClosed& ev);
    void process(const Ctrl::UslDiceThrowStart& ev);
    void process(const Ctrl::UslDiceNewValue& ev);
    void process(const Ctrl::UslResolveBetsEnd& ev);
    void process(const Ctrl::UslPointEstablished& ev);
    void process(const Ctrl::UslSevenOut& ev);
    void process(const Ctrl::UslPassLineWinner& ev);
    void process(const Ctrl::UslNewShooter& ev);
    void process(const Ctrl::UslBettingOpened& ev);
//  void process(const Ctrl::UslCountdownRollDice& ev);
    
    void process(const Ctrl::UslBetMade& ev);
    void process(const Ctrl::UslBetResolved& ev);
    void process(const Ctrl::UslBetRemoved& ev);
    void process(const Ctrl::UslRollHistoryFormatChanged& ev);
    void process(const Ctrl::UslBetChanged& ev);
    void process(const Ctrl::UslAutoFillChanged& ev);
    void process(const Ctrl::UslQuickBetChanged& ev);
    
    void process(const Ctrl::UslTableBetsChanged& ev);
    void process(const Ctrl::UslTableSessionStatsChanged& ev);
    void process(const Ctrl::UslTableLastRollStatsChanged& ev);
    void process(const Ctrl::UslTableBalanceChanged& ev);

    void process(const Ctrl::UslPlayerBetsChanged& ev);
    void process(const Ctrl::UslPlayerSessionStatsChanged& ev);
    void process(const Ctrl::UslPlayerLastRollStatsChanged& ev);
    void process(const Ctrl::UslPlayerBalanceChanged& ev);

    void process(const Ctrl::UslPlayerJoinedTable& ev);
    void process(const Ctrl::UslPlayerLeftTable& ev);
    void process(const Ctrl::UslUserPlayerSwitched& ev);
    void process(const Ctrl::UslPlayerRenamed& ev);
    
    void process(const Ctrl::RspMakeBet& ev);
    void process(const Ctrl::RspOddsBet& ev);
    void process(const Ctrl::RspRemoveBet& ev);
    void process(const Ctrl::RspUndoBet& ev);
    void process(const Ctrl::RspSwitchPlayer& ev);
    void process(const Ctrl::RspManagePlayer& ev);

private:
    void refreshRoster();

    // While the dice animation plays, the results of the roll are held
    // back so the person does not see them before the dice land.
    static bool passesThrough(const WorkOrderEvent& woe);
    void        holdOrRun(std::function<void()> fn);

    static constexpr std::chrono::seconds MaxHold{10};

    bool holding_ = false;
    std::chrono::steady_clock::time_point holdStart_;
    std::vector<std::function<void()>>    held_;
};

/*-----------------------------------------------------------*//**

@class WorkOrderDispatcher

@brief Dispatches work orders to the many CUI presentation classes

Receives all WorkOrders coming off the CuiThread.

The model resolves a roll in one burst, but the person should first
watch the dice land. From UslDiceThrowStart until the animation reports
it is done, result events are queued here and then replayed in their
original order. Events that must not wait (the animation's own dice
values, responses to requests) pass straight through.

 Translates work orders
and GameEvents into primitive data types, then calls into various
windows using hard coded knowledge of which window wants what
information.
*/

} // namespace Cui

//----------------------------------------------------------------
