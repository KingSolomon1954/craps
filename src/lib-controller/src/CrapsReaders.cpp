//----------------------------------------------------------------
//
// File: CrapsReaders.cpp
//
//----------------------------------------------------------------

#include <controller/CrapsReaders.h>
#include <controller/AutoFill.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
#include <controller/QuickBet.h>
#include <controller/TableManager.h>
#include <controller/UndoManager.h>
#include <craps/BankStats.h>
#include <craps/CrapsTable.h>
#include <craps/Dice.h>
#include <craps/LastRollStats.h>
#include <craps/Player.h>
#include <craps/PlayerStats.h>
#include <craps/SessionHistory.h>
#include <craps/TableStats.h>

using namespace Ctrl;

//----------------------------------------------------------------
//
// Player related
//
//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readPlayerHaveBet(
    const Craps::PlayerId& playerId,
    const Craps::BetId& betId,
    bool& haveBet,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("playerHaveBet", "retrieve player bet"));
        Gen::ReturnCode::Fail;
        haveBet = false;
        return;
    }
    haveBet = (pBet->player().getPlayerId() == playerId);
    return Gen::ReturnCode::Success;    
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readPlayerGetBets(
    const Craps::PlayerId& playerId,
    std::vector<Craps::BetId>& betIds,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend(diagPrefix("playerName", "retrieve player bets"));
        return Gen::ReturnCode::Fail;
    }

    auto playerBets = p->getBets();  // Grab player's bets
    betIds.clear();
    for (auto b : playerBets)        // Build a list of BetIds
    {
        betIds.push_back(b->betId());
    }
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readPlayerGetOddsBets(
    const Craps::PlayerId& playerId,
    std::vector<Craps::BetId>& betIds,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend(diagPrefix("playerName", "retrieve player bets"));
        return Gen::ReturnCode::Fail;
    }

    auto playerBets = p->getOddsBets();  // Grab player's odds bets
    betIds.clear();
    for (auto b : playerBets)            // Build a list of BetIds
    {
        betIds.push_back(b->betId());
    }
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readPlayerName(
    const Craps::PlayerId& playerId,
    std::string& playerName,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend(diagPrefix("playerName", "retrieve player name"));
        return Gen::ReturnCode::Fail;
    }
    playerName = p->getName();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readPlayerAmountOnTable(
    const Craps::PlayerId& playerId,
    Gen::Money& amount,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend(diagPrefix("playerAmountOnTable", "retrieve amount on table"));
        return Gen::ReturnCode::Fail;
    }
    amount = p->getAmountOnTable();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readPlayerNumBetsOnTable(
    const Craps::PlayerId& playerId,
    unsigned& numBets,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend(diagPrefix("playerAmountOnTable", "retrieve amount on table"));
        return Gen::ReturnCode::Fail;
    }
    numBets = p->getNumBetsOnTable();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readPlayerBalance(
    const Craps::PlayerId& playerId,
    Gen::Money& balance,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend(diagPrefix("playerBalance", "retrieve player balance"));
        return Gen::ReturnCode::Fail;
    }
    balance = p->getBalance();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readPlayerLastRollStats(
    const Craps::PlayerId& playerId,
    Craps::LastRollStats& lastRollStats,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend(diagPrefix("playerLastRollStats", "retrieve last roll stats"));
        return Gen::ReturnCode::Fail;
    }
    lastRollStats = p->getLastRollStats();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readPlayerCurrentStats(
    const Craps::PlayerId& playerId,
    Craps::PlayerStats& playerCurrentStats,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend(diagPrefix("playerCurrentStats", "retrieve player current stats"));
        return Gen::ReturnCode::Fail;
    }
    playerCurrentStats = p->getCurrentStats();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readPlayerAlltimeStats(
    const Craps::PlayerId& playerId,
    Craps::PlayerStats& playerAlltimeStats,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend(diagPrefix("playerAlltimeStats", "retrieve player alltime stats"));
        return Gen::ReturnCode::Fail;
    }
    playerAlltimeStats = p->getAlltimeStats();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readPlayerCurrentStatsBank(
    const Craps::PlayerId& playerId,
    Craps::BankStats& playerCurrentStatsBank,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend(diagPrefix("playerCurrentStatsBank", "retrieve player currennt bank stats"));
        return Gen::ReturnCode::Fail;
    }
    playerCurrentStatsBank = p->getBankCurrentStats();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readPlayerSessionHistory(
    const Craps::PlayerId& playerId,
    Craps::SessionHistory::Sessions& playerSessionHistory,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend(diagPrefix("playerSessionHistory", "retrieve player session history"));
        return Gen::ReturnCode::Fail;
    }
    playerSessionHistory = p->getSessionHistory();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

const PlayerManifest::PlayerList&
CrapsReaders::readPlayerList()
{
    return Gbl::pPlayerMgr->getPlayerList();
}

//----------------------------------------------------------------
//
// Bet related
//
//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readBetIsRemovable(
    const Craps::BetId& betId,
    bool& isRemovable,
    Gen::ErrorPass& ep)    
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("beIsRemovable", "retrieve bet"));
        return Gen::ReturnCode::Fail;
    }
    isRemovable = Gbl::pTable->isBetRemovable(pBet);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readBetPivot(
    const Craps::BetId& betId,
    unsigned& pivot,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betPivot", "retrieve pivot"));
        return Gen::ReturnCode::Fail;
    }
    pivot = pBet->pivot();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readBetPlayerId(
    const Craps::BetId& betId,
    Craps::PlayerId& playerId,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betPlayerId", "retrieve playerId from bet"));
        return Gen::ReturnCode::Fail;
    }
    playerId = pBet->player().getPlayerId();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readBetName(
    const Craps::BetId& betId,
    BetName& betName,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betName", "retrieve bet name"));
        return Gen::ReturnCode::Fail;
    }
    betName = pBet->betName();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readBetContractAmount(
    const Craps::BetId& betId,
    Gen::Money& contractAmount,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betContractAmount", "retrieve contract amount"));
        return Gen::ReturnCode::Fail;
    }
    contractAmount = pBet->contractAmount();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readBetOddsAmount(
    const Craps::BetId& betId,
    Gen::Money& oddsAmount,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betContractAmount", "retrieve odds amount"));
        return Gen::ReturnCode::Fail;
    }
    oddsAmount = pBet->oddsAmount();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readBetOffComeOutRoll(
    const Craps::BetId& betId,
    bool& offComeOutRoll,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betOffComeOutRoll", "retrieve comeout roll flag"));
        return Gen::ReturnCode::Fail;
    }
    offComeOutRoll = pBet->offComeOutRoll();
    return Gen::ReturnCode::Success;
    
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readBetHardwayWorking(
    const Craps::BetId& betId,
    bool& hardwayWorking,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betHardwayWorking", "retrieve hardway working flag"));
        return Gen::ReturnCode::Fail;
    }
    hardwayWorking = pBet->hardwayWorking();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readBetDistance(
    const Craps::BetId& betId,
    unsigned& distance,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betDistance", "retrieve distance"));
        return Gen::ReturnCode::Fail;
    }
    distance = pBet->distance();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readBetWhenCreated(
    const Craps::BetId& betId,
    Gen::Timepoint& whenCreated,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betWhenCreated", "retrieve when created"));
        return Gen::ReturnCode::Fail;
    }
    whenCreated = pBet->whenCreated();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readBetWhenDecided(
    const Craps::BetId& betId,
    Gen::Timepoint& whenDecided,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betWhenDecided", "retrieve when decided"));
        return Gen::ReturnCode::Fail;
    }
    whenDecided = pBet->whenDecided();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------
//
// Table related
//
//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTableNumPlayers(
    const Craps::TableId& tableId,
    unsigned& numPlayers,
    Gen::ErrorPass& ep)
{
    auto pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tableNumPlayers", "retrieve num players"));
        return Gen::ReturnCode::Fail;
    }
    numPlayers = pTable->getNumPlayers();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTablePoint(
    const Craps::TableId& tableId,
    unsigned& point,
    Gen::ErrorPass& ep)
{
    auto pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tablePoint", "retrieve point"));
        return Gen::ReturnCode::Fail;
    }
    point = pTable->getPoint();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTableCurrentRoll(
    const Craps::TableId& tableId,
    Craps::Dice& dice,
    Gen::ErrorPass& ep)
{
    auto pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tableCurrentRoll", "retrieve current roll"));
        return Gen::ReturnCode::Fail;
    }
    dice = pTable->getCurrentRoll();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTableNumRolls(
    const Craps::TableId& tableId,
    unsigned& numRolls,
    Gen::ErrorPass& ep)
{
    Craps::CrapsTable* pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tableNumRolls", "retrieve num rolls"));
        return Gen::ReturnCode::Fail;
    }
    numRolls = pTable->getNumRolls();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTableAmountOnTable(
    const Craps::TableId& tableId,
    Gen::Money& amount,
    Gen::ErrorPass& ep)
{
    auto pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tableAmountOnTable", "retrieve amount"));
        return Gen::ReturnCode::Fail;
    }
    amount = pTable->getAmountOnTable();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTableNumBetsOnTable(
    const Craps::TableId& tableId,
    unsigned& numBets,
    Gen::ErrorPass& ep)
{
    Craps::CrapsTable* pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tableNumBetsOnTable", "retrieve num bets"));
        return Gen::ReturnCode::Fail;
    }
    numBets = pTable->getNumBetsOnTable();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTablePlayers(
    const Craps::TableId& tableId,
    std::vector<Craps::PlayerId>& playerIds,
    Gen::ErrorPass& ep)
{
    Craps::CrapsTable* pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tablePlayers", "retrieve table players"));
        return Gen::ReturnCode::Fail;
    }
    const auto playerPtrs  = pTable->getPlayers();
    for (const auto p : playerPtrs)
    {
        playerIds.push_back(p->getPlayerId());
    }
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTableCurrentShooter(
    const Craps::TableId& tableId,
    Craps::PlayerId& playerId,
    Gen::ErrorPass& ep)
{
    Craps::CrapsTable* pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tableCurrentShooter", "retrieve current shooter"));
        return Gen::ReturnCode::Fail;
    }
    playerId = pTable->getCurrentShooter()->getPlayerId();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTableRecentRolls(
    const Craps::TableId& tableId,
    std::deque<Craps::Dice>& recentRolls,
    Gen::ErrorPass& ep)
{
    Craps::CrapsTable* pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tableRecentRolls", "retrieve recent rolls"));
        return Gen::ReturnCode::Fail;
    }
    recentRolls = pTable->getRecentRolls();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTableCurrentStats(
    const Craps::TableId& tableId,
    Craps::TableStats& currentTableStats,
    Gen::ErrorPass& ep)
{
    Craps::CrapsTable* pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tableCurrentStats", "retrieve current stats"));
        return Gen::ReturnCode::Fail;
    }
    currentTableStats = pTable->getCurrentStats();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTableAlltimeStats(
    const Craps::TableId& tableId,
    Craps::TableStats& alltimeTableStats,
    Gen::ErrorPass& ep)
{
    Craps::CrapsTable* pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tableAlltimeStats", "retrieve alltime stats"));
        return Gen::ReturnCode::Fail;
    }
    alltimeTableStats = pTable->getAlltimeStats();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTableCurrentStatsBank(
    const Craps::TableId& tableId,
    Craps::BankStats& currentStatsBank,
    Gen::ErrorPass& ep)
{
    Craps::CrapsTable* pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tableCurrentStatsBank", "retrieve current bank stats"));
        return Gen::ReturnCode::Fail;
    }
    currentStatsBank = pTable->getBankCurrentStats();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTableAlltimeStatsBank(
    const Craps::TableId& tableId,
    Craps::BankStats& alltimeStatsBank,
    Gen::ErrorPass& ep)
{
    Craps::CrapsTable* pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tableAlltimeStatsBank", "retrieve alltime bank stats"));
        return Gen::ReturnCode::Fail;
    }
    alltimeStatsBank = pTable->getBankAlltimeStats();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTableLastRollStats(
    const Craps::TableId& tableId,
    Craps::LastRollStats& lastRollStats,
    Gen::ErrorPass& ep)
{
    Craps::CrapsTable* pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tableLastRollStats", "retrieve last roll stats"));
        return Gen::ReturnCode::Fail;
    }
    lastRollStats = pTable->getLastRollStats();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTableSessionHistory(
    const Craps::TableId& tableId,
    Craps::SessionHistory::Sessions& tableSessionHistory,
    Gen::ErrorPass& ep)
{
    Craps::CrapsTable* pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tableSessionHistory", "retrieve session history"));
        return Gen::ReturnCode::Fail;
    }
    tableSessionHistory = pTable->getSessionHistory();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTableIsComeOutRoll(
    const Craps::TableId& tableId,
    bool& isComeOutRoll,
    Gen::ErrorPass& ep)
{
    Craps::CrapsTable* pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tableComeOutRoll", "retrieve come out roll flag"));
        return Gen::ReturnCode::Fail;
    }
    isComeOutRoll = pTable->isComeOutRoll();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTableIsBettingOpen(
    const Craps::TableId& tableId,
    bool& isBettingOpen,
    Gen::ErrorPass& ep)
{
    Craps::CrapsTable* pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tableBettingOpen", "retrieve betting opne flag"));
        return Gen::ReturnCode::Fail;
    }
    isBettingOpen = pTable->isBettingOpen();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTableHaveBet(
    const Craps::TableId& tableId,
    const Craps::BetId& betId,
    bool& haveBet,
    Gen::ErrorPass& ep)
{
    Craps::CrapsTable* pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("tableHaveBet", "retrieve table bet"));
        return Gen::ReturnCode::Fail;
    }
    auto pBet = Gbl::pTable->getBet(betId, ep);
    haveBet = (pBet != nullptr);
    return Gen::ReturnCode::Success;    
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::readTableHavePlayer(
    const Craps::TableId& tableId,
    const Craps::PlayerId& playerId,
    bool& havePlayer,
    Gen::ErrorPass& ep)
{
    auto pTable  = Gbl::pTableMgr->getTable  (tableId,  ep);
    auto pPlayer = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (pTable == nullptr || pPlayer == nullptr)
    {
        ep.prepend(diagPrefix("tableHavePlayer", "retrieve table player"));
        return Gen::ReturnCode::Fail;
    }
    havePlayer = pTable->havePlayer(pPlayer);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

const TableManifest::TableList&
CrapsReaders::readTableList()
{
    return Gbl::pTableMgr->getTableList();
}

//----------------------------------------------------------------
//
// Auto Fills
//
//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::getAutoFill(
    AutoFillEntry& entry,
    Gen::ErrorPass& ep)
{
    AutoFill::AutoFillEntry afe =
        {entry.betName, entry.pivot, entry.oddsBet, entry.amount };
    
    if (AutoFill::instance()->getAutoFill(afe) == Gen::ReturnCode::Success)
    {
        entry.amount = afe.amount;
        return Gen::ReturnCode::Fail;
    }
    ep.diag = "No such auto fill entry exists.";
    return Gen::ReturnCode::Fail;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::getAutoFills(
    std::vector<AutoFillEntry>& autoFills,  // return arg
    Gen::ErrorPass& ep)
{
    AutoFill::Fills controllerFills = AutoFill::instance()->getAutoFills();

    for (size_t i = 0; i < controllerFills.size(); i++)
    {
        autoFills[i].betName = controllerFills[i].betName;
        autoFills[i].pivot   = controllerFills[i].pivot;
        autoFills[i].oddsBet = controllerFills[i].isOddsBet;
        autoFills[i].amount  = controllerFills[i].amount;
    }
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------
//
// Quick Bets
//
//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::getQuickBets(
    std::vector<QuickBetEntry>& quickBets,  // return arg
    Gen::ErrorPass& ep)
{
    QuickBet::Bets controllerBets = QuickBet::instance()->getQuickBets();

    for (size_t i = 0; i < controllerBets.size(); i++)
    {
        quickBets[i].betName = controllerBets[i].betName;
        quickBets[i].pivot   = controllerBets[i].pivot;
        quickBets[i].oddsBet = controllerBets[i].isOddsBet;
        quickBets[i].amount  = controllerBets[i].amount;
    }
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------
//
// Program related
//
//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::getActiveCrapsTable(
    Craps::TableId& tableId,
    Gen::ErrorPass& ep)
{
    auto* t = Gbl::pTable;
    if (t == nullptr)
    {
        ep.diag = "CrapsReaders::getActiveCrapsTable(): nullptr for active table. Coding error.";
        return Gen::ReturnCode::Fail;
    }
    tableId = t->getTableId();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsReaders::getUserPlayer(
    Craps::PlayerId& playerId,
    Gen::ErrorPass& ep)
{
    auto* p = Gbl::pPlayerMgr->getUserPlayer();
    if (p == nullptr)
    {
        ep.diag = "CrapsReaders::getUserPlayer(): nullptr for user/player. Coding error.";
        return Gen::ReturnCode::Fail;
    }
    playerId = p->getPlayerId();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

// Private helper
std::string
CrapsReaders::diagPrefix(
    const std::string& funcName,
    const std::string& unableToWhat)
{
    std::string d = "CrapsReaders::" + funcName + 
                    "(): unable to " + unableToWhat + "; ";
    return d;
}

//----------------------------------------------------------------

