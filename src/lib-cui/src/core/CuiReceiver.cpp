//----------------------------------------------------------------
//
// File: CuiReceiver.cpp
//
//----------------------------------------------------------------

#include <cui/core/CuiReceiver.h>
#include <cui/core/WorkOrder.h>
#include <cui/core/CuiThread.h>
#include <craps/CrapsTypes.h>
#include <gen/EventManager.h>
#include <gen/Logger.h>

using namespace Cui;

//----------------------------------------------------------------

CuiReceiver&
CuiReceiver::instance()
{
    static CuiReceiver cr;
    return cr;
}

//----------------------------------------------------------------

CuiReceiver::CuiReceiver()
{
    Gen::EventManager::instance().subscribe<Ctrl::RspMakeBet>(
        [this](const Ctrl::RspMakeBet& ev)
        {
            this->onRspMakeBet(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::RspOddsBet>(
        [this](const Ctrl::RspOddsBet& ev)
        {
            this->onRspOddsBet(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::RspSwitchPlayer>(
        [this](const Ctrl::RspSwitchPlayer& ev)
        {
            this->onRspSwitchPlayer(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::RspManagePlayer>(
        [this](const Ctrl::RspManagePlayer& ev)
        {
            this->onRspManagePlayer(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslPlayerJoinedTable>(
        [this](const Ctrl::UslPlayerJoinedTable& ev)
        {
            this->onPlayerJoinedTable(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslPlayerLeftTable>(
        [this](const Ctrl::UslPlayerLeftTable& ev)
        {
            this->onPlayerLeftTable(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslUserPlayerSwitched>(
        [this](const Ctrl::UslUserPlayerSwitched& ev)
        {
            this->onUserPlayerSwitched(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslPlayerRenamed>(
        [this](const Ctrl::UslPlayerRenamed& ev)
        {
            this->onPlayerRenamed(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslBettingClosed>(
        [this](const Ctrl::UslBettingClosed& ev)
        {
            this->onBettingClosed(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslDiceThrowStart>(
        [this](const Ctrl::UslDiceThrowStart& ev)
        {
            this->onDiceThrowStart(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslDiceNewValue>(
        [this](const Ctrl::UslDiceNewValue& ev)
        {
            this->onDiceNewValue(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslResolveBetsEnd>(
        [this](const Ctrl::UslResolveBetsEnd& ev)
        {
            this->onResolveBetsEnd(ev);
        });
    
    Gen::EventManager::instance().subscribe<Ctrl::UslPointEstablished>(
        [this](const Ctrl::UslPointEstablished& ev)
        {
            this->onPointEstablished(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslSevenOut>(
        [this](const Ctrl::UslSevenOut& ev)
        {
            this->onSevenOut(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslPassLineWinner>(
        [this](const Ctrl::UslPassLineWinner& ev)
        {
            this->onPassLineWinner(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslNewShooter>(
        [this](const Ctrl::UslNewShooter& ev)
        {
            this->onNewShooter(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslBettingOpened>(
        [this](const Ctrl::UslBettingOpened& ev)
        {
            this->onBettingOpened(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslBetMade>(
        [this](const Ctrl::UslBetMade& ev)
        {
            this->onBetMade(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslBetChanged>(
        [this](const Ctrl::UslBetChanged& ev)
        {
            this->onBetChanged(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::RspRemoveBet>(
        [this](const Ctrl::RspRemoveBet& ev)
        {
            this->onRspRemoveBet(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::RspUndoBet>(
        [this](const Ctrl::RspUndoBet& ev)
        {
            this->onRspUndoBet(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::RspApplyQuickBets>(
        [this](const Ctrl::RspApplyQuickBets& ev)
        {
            this->onRspApplyQuickBets(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::RspEditQuickBets>(
        [this](const Ctrl::RspEditQuickBets& ev)
        {
            this->onRspEditQuickBets(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslRollHistoryFormatChanged>(
        [this](const Ctrl::UslRollHistoryFormatChanged& ev)
        {
            this->onRollHistoryFormatChanged(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslBetRemoved>(
        [this](const Ctrl::UslBetRemoved& ev)
        {
            this->onBetRemoved(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslBetResolved>(
        [this](const Ctrl::UslBetResolved& ev)
        {
            this->onBetResolved(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslTableBetsChanged>(
        [this](const Ctrl::UslTableBetsChanged& ev)
        {
            this->onTableBetsChanged(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslTableSessionStatsChanged>(
        [this](const Ctrl::UslTableSessionStatsChanged& ev)
        {
            this->onTableSessionStatsChanged(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslTableLastRollStatsChanged>(
        [this](const Ctrl::UslTableLastRollStatsChanged& ev)
        {
            this->onTableLastRollStatsChanged(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslTableBalanceChanged>(
        [this](const Ctrl::UslTableBalanceChanged& ev)
        {
            this->onTableBalanceChanged(ev);
        });
    
    Gen::EventManager::instance().subscribe<Ctrl::UslPlayerBetsChanged>(
        [this](const Ctrl::UslPlayerBetsChanged& ev)
        {
            this->onPlayerBetsChanged(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslPlayerSessionStatsChanged>(
        [this](const Ctrl::UslPlayerSessionStatsChanged& ev)
        {
            this->onPlayerSessionStatsChanged(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslPlayerLastRollStatsChanged>(
        [this](const Ctrl::UslPlayerLastRollStatsChanged& ev)
        {
            this->onPlayerLastRollStatsChanged(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::UslPlayerBalanceChanged>(
        [this](const Ctrl::UslPlayerBalanceChanged& ev)
        {
            this->onPlayerBalanceChanged(ev);
        });
}

//----------------------------------------------------------------

void
CuiReceiver::onRspMakeBet(const Ctrl::RspMakeBet& ev) const
{
    LOG_TRACE("CuiReceiver::onRspMakeBet()");
    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(std::move(wo));
}

//----------------------------------------------------------------

void
CuiReceiver::onRspOddsBet(const Ctrl::RspOddsBet& ev) const
{
    LOG_TRACE("CuiReceiver::onRspOddsBet()");
    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(std::move(wo));
}

//----------------------------------------------------------------

void
CuiReceiver::onBettingClosed(const Ctrl::UslBettingClosed& ev) const
{
    LOG_TRACE("CuiReceiver()::onBettingClosed()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onDiceThrowStart(const Ctrl::UslDiceThrowStart& ev) const
{
    LOG_TRACE("CuiReceiver()::onDiceThrowStart()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onDiceNewValue(const Ctrl::UslDiceNewValue& ev) const
{
    LOG_TRACE("CuiReceiver()::onDiceNewValue()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);

    std::string s = "CuiReceiver::onDiceNewValue() Dice " +
                    std::to_string(ev.val) + " ("         +
                    std::to_string(ev.d1)  + ","          +
                    std::to_string(ev.d2)  + ") Roll: "   +
                    std::to_string(ev.rollCount);
    LOG_TRACE(s);
}

//----------------------------------------------------------------

void
CuiReceiver::onResolveBetsEnd(const Ctrl::UslResolveBetsEnd& ev) const
{
    LOG_TRACE("CuiReceiver()::onResolveBetsEnd()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPointEstablished(const Ctrl::UslPointEstablished& ev) const
{
    LOG_TRACE("CuiReceiver()::onPointEstablished()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onSevenOut(const Ctrl::UslSevenOut& ev) const
{
    LOG_TRACE("CuiReceiver()::onSevenOut()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPassLineWinner(const Ctrl::UslPassLineWinner& ev) const
{
    LOG_TRACE("CuiReceiver()::onPassLineWinner()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onNewShooter(const Ctrl::UslNewShooter& ev) const
{
    LOG_TRACE("CuiReceiver()::onNewShooter()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onBettingOpened(const Ctrl::UslBettingOpened& ev) const
{
    LOG_TRACE("CuiReceiver()::onBettingOpened()");
    
    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onBetMade(const Ctrl::UslBetMade& ev) const
{
    LOG_TRACE("CuiReceiver()::onBetMade()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onBetChanged(const Ctrl::UslBetChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onBetChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onRspUndoBet(const Ctrl::RspUndoBet& ev) const
{
    LOG_TRACE("CuiReceiver::onRspUndoBet()");
    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(std::move(wo));
}

//----------------------------------------------------------------

void
CuiReceiver::onRspApplyQuickBets(const Ctrl::RspApplyQuickBets& ev) const
{
    LOG_TRACE("CuiReceiver::onRspApplyQuickBets()");
    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(std::move(wo));
}

//----------------------------------------------------------------

void
CuiReceiver::onRspEditQuickBets(const Ctrl::RspEditQuickBets& ev) const
{
    LOG_TRACE("CuiReceiver::onRspEditQuickBets()");
    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(std::move(wo));
}

//----------------------------------------------------------------

void
CuiReceiver::onRspRemoveBet(const Ctrl::RspRemoveBet& ev) const
{
    LOG_TRACE("CuiReceiver::onRspRemoveBet()");
    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(std::move(wo));
}

//----------------------------------------------------------------

void
CuiReceiver::onRollHistoryFormatChanged(
    const Ctrl::UslRollHistoryFormatChanged& ev) const
{
    LOG_TRACE("CuiReceiver::onRollHistoryFormatChanged()");
    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(std::move(wo));
}

//----------------------------------------------------------------

void
CuiReceiver::onBetRemoved(const Ctrl::UslBetRemoved& ev) const
{
    LOG_TRACE("CuiReceiver()::onBetRemoved()");
    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(std::move(wo));
}

//----------------------------------------------------------------

void
CuiReceiver::onBetResolved(const Ctrl::UslBetResolved& ev) const
{
    LOG_TRACE("CuiReceiver()::onBetResolved()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onTableBetsChanged (const Ctrl::UslTableBetsChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onTableBetsChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onTableLastRollStatsChanged(const Ctrl::UslTableLastRollStatsChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onTableLastRollStatsChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}
   
//----------------------------------------------------------------

void
CuiReceiver::onTableSessionStatsChanged(const Ctrl::UslTableSessionStatsChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onTableSessionStatsChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onTableBalanceChanged(const Ctrl::UslTableBalanceChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onTableBalanceChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPlayerBetsChanged (const Ctrl::UslPlayerBetsChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onPlayerBetsChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPlayerLastRollStatsChanged(const Ctrl::UslPlayerLastRollStatsChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onPlayerLastRollStatsChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}
   
//----------------------------------------------------------------

void
CuiReceiver::onPlayerSessionStatsChanged(const Ctrl::UslPlayerSessionStatsChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onPlayerSessionStatsChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPlayerBalanceChanged(const Ctrl::UslPlayerBalanceChanged& ev) const
{
    LOG_TRACE("CuiReceiver()::onPlayerBalanceChanged()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

//----------------------------------------------------------------

//----------------------------------------------------------------

void
CuiReceiver::onRspSwitchPlayer(const Ctrl::RspSwitchPlayer& ev) const
{
    LOG_TRACE("CuiReceiver()::onRspSwitchPlayer()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onRspManagePlayer(const Ctrl::RspManagePlayer& ev) const
{
    LOG_TRACE("CuiReceiver()::onRspManagePlayer()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPlayerJoinedTable(const Ctrl::UslPlayerJoinedTable& ev) const
{
    LOG_TRACE("CuiReceiver()::onPlayerJoinedTable()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPlayerLeftTable(const Ctrl::UslPlayerLeftTable& ev) const
{
    LOG_TRACE("CuiReceiver()::onPlayerLeftTable()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onUserPlayerSwitched(const Ctrl::UslUserPlayerSwitched& ev) const
{
    LOG_TRACE("CuiReceiver()::onUserPlayerSwitched()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------

void
CuiReceiver::onPlayerRenamed(const Ctrl::UslPlayerRenamed& ev) const
{
    LOG_TRACE("CuiReceiver()::onPlayerRenamed()");

    WorkOrderEvent wo{.event = ev};
    CuiThread::instance().enqueueWork(wo);
}

//----------------------------------------------------------------
