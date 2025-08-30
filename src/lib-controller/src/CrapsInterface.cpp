//----------------------------------------------------------------
//
// File: CrapsInterface.cpp
//
//----------------------------------------------------------------

#include <controller/CrapsInterface.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
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
CrapsInterface::playerJoinTable(
    const Craps::PlayerId& playerId,
    const Craps::TableId& tableId,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend(diagPrefix("playerJoinTable", "join table"));
        return Gen::ReturnCode::Fail;
    }
    return p->joinTable(Gbl::pTable, ep);
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::playerLeaveTable(
    const Craps::PlayerId& playerId,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend(diagPrefix("leaveTable", "leave table"));
        return Gen::ReturnCode::Fail;
    }
    return p->leaveTable(ep);
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::playerMakeBet(
    const Craps::PlayerId& playerId,
    BetName betName,
    Gen::Money contractAmount,
    unsigned pivot,
    Craps::BetId& betId,
    Gen::ErrorPass& ep)
{
    Craps::Player* p = Gbl::pPlayerMgr->getPlayer(playerId, ep);
    if (p == nullptr)
    {
        ep.prepend(diagPrefix("playerMakeBet", "make bet"));
        return Gen::ReturnCode::Fail;
    }
    Craps::BetPtr pBet = p->makeBet(betName, contractAmount, pivot, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("playerMakeBet", "make bet"));
        return Gen::ReturnCode::Fail;
    }
    Gbl::pUndoMgr->push(std::make_unique<UndoBetAdded>(pBet));
    betId = pBet->betId();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::playerHaveBet(
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
    }
    haveBet = (pBet->player().getPlayerId() == playerId);
    if (haveBet)
    {
        return Gen::ReturnCode::Fail;
    }
    return Gen::ReturnCode::Success;    
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::playerName(
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
CrapsInterface::playerAmountOnTable(
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
CrapsInterface::playerNumBetsOnTable(
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
CrapsInterface::playerBalance(
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
CrapsInterface::playerLastRollStats(
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
CrapsInterface::playerCurrentStats(
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
CrapsInterface::playerAlltimeStats(
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
CrapsInterface::playerCurrentStatsBank(
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
CrapsInterface::playerSessionHistory(
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
CrapsInterface::playerList()
{
    return Gbl::pPlayerMgr->getPlayerList();
}

//----------------------------------------------------------------
//
// Bet related
//
//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betSetOddsAmount(
    const Craps::BetId& betId,
    Gen::Money oddsAmount,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betSetOddsAmount", "setOddsAmount"));
        return Gen::ReturnCode::Fail;
    }
    Gbl::pUndoMgr->push(std::make_unique<UndoBetModifiedAmount>(pBet));
    return pBet->player().setOddsAmount(pBet, oddsAmount, ep);
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betSetContractAmount(
    const Craps::BetId& betId,
    Gen::Money contractAmount,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betSetContractAmount", "setContractAmount"));
        return Gen::ReturnCode::Fail;
    }
    Gbl::pUndoMgr->push(std::make_unique<UndoBetModifiedAmount>(pBet));
    return pBet->player().setContractAmount(betId, contractAmount, ep);
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betIsRemovable(
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
CrapsInterface::betRemove(
    const Craps::BetId& betId,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betRemove", "remove bet"));
        return Gen::ReturnCode::Fail;
    }
    Gbl::pUndoMgr->push(std::make_unique<UndoBetRemoved>(pBet));
    return pBet->player().removeBet(betId, ep);
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betPivot(
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
CrapsInterface::betPlayerId(
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
CrapsInterface::betName(
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
CrapsInterface::betContractAmount(
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
CrapsInterface::betOddsAmount(
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
CrapsInterface::betOffComeOutRoll(
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
CrapsInterface::betHardwayWorking(
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
CrapsInterface::betDistance(
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
CrapsInterface::betWhenCreated(
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
CrapsInterface::betWhenDecided(
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

Gen::ReturnCode
CrapsInterface::betSetOffComeOutRoll(
    const Craps::BetId& betId,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betSetOffComeOutRoll", "set offComeOutRoll flag"));
        return Gen::ReturnCode::Fail;
    }
    Gbl::pUndoMgr->push(std::make_unique<UndoBetModifiedFlags>(pBet));
    pBet->setOffComeOutRoll();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betSetOnComeOutRoll(
    const Craps::BetId& betId,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betSetOnComeOutRoll", "set onComeOutRoll flag"));
        return Gen::ReturnCode::Fail;
    }
    Gbl::pUndoMgr->push(std::make_unique<UndoBetModifiedFlags>(pBet));
    pBet->setOnComeOutRoll();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betSetHardwayOff(
    const Craps::BetId& betId,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betSetHardwayOff", "set hardway off flag"));
        return Gen::ReturnCode::Fail;
    }
    Gbl::pUndoMgr->push(std::make_unique<UndoBetModifiedFlags>(pBet));
    pBet->setHardwayOff();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::betSetHardwayOn(
    const Craps::BetId& betId,
    Gen::ErrorPass& ep)
{
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betSetHardwayOn", "set hardway on flag"));
        return Gen::ReturnCode::Fail;
    }
    Gbl::pUndoMgr->push(std::make_unique<UndoBetModifiedFlags>(pBet));
    pBet->setHardwayOn();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------
//
// Table related
//
//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::rollDice(
    const Craps::TableId& tableId,
    Gen::ErrorPass& ep)
{
    auto pTable = Gbl::pTableMgr->getTable(tableId, ep);
    if (pTable == nullptr)
    {
        ep.prepend(diagPrefix("rollDice", "roll dice"));
        return Gen::ReturnCode::Fail;
    }
    pTable->rollDice();
    Gbl::pUndoMgr->clear();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::tableNumPlayers(
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
CrapsInterface::tablePoint(
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
CrapsInterface::tableCurrentRoll(
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
CrapsInterface::tableNumRolls(
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
CrapsInterface::tableAmountOnTable(
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
CrapsInterface::tableNumBetsOnTable(
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
CrapsInterface::tablePlayers(
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
CrapsInterface::tableCurrentShooter(
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
CrapsInterface::tableRecentRolls(
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
CrapsInterface::tableCurrentStats(
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
CrapsInterface::tableAlltimeStats(
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
CrapsInterface::tableCurrentStatsBank(
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
CrapsInterface::tableAlltimeStatsBank(
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
CrapsInterface::tableLastRollStats(
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
CrapsInterface::tableSessionHistory(
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
CrapsInterface::tableIsComeOutRoll(
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
CrapsInterface::tableIsBettingOpen(
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
CrapsInterface::tableHaveBet(
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
CrapsInterface::tableHavePlayer(
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
CrapsInterface::tableList()
{
    return Gbl::pTableMgr->getTableList();
}

//----------------------------------------------------------------
//
// Program related
//
//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::getActiveCrapsTable(
        Craps::TableId& tableId,
        Gen::ErrorPass& ep)
{
    auto* t = Gbl::pTable;
    if (t == nullptr)
    {
        ep.diag = "CrapsInterface::getActiveCrapsTable(): nullptr for active table. Coding error.";
        return Gen::ReturnCode::Fail;
    }
    tableId = t->getTableId();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::getUserPlayer(
        Craps::PlayerId& playerId,
        Gen::ErrorPass& ep)
{
    auto* p = Gbl::pPlayerMgr->getUserPlayer();
    if (p == nullptr)
    {
        ep.diag = "CrapsInterface::getUserPlayer(): nullptr for user/player. Coding error.";
        return Gen::ReturnCode::Fail;
    }
    playerId = p->getPlayerId();
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsInterface::undoLast(
    Gen::ErrorPass& ep)
{
    if (Gbl::pUndoMgr->canUndo())
    {
        Gbl::pUndoMgr->undo();
        return Gen::ReturnCode::Success;
    }
    ep.diag = "CrapsInterface::betUndoLast(): Nothing to undo. "
              "Undo stack is empty.";
    return Gen::ReturnCode::Fail;
}

//----------------------------------------------------------------

void
CrapsInterface::gameTerminate()
{
    // TODO
}

//----------------------------------------------------------------

// Private helper
std::string
CrapsInterface::diagPrefix(
    const std::string& funcName,
    const std::string& unableToWhat)
{
    std::string d = "CrapsInterface::" + funcName + 
                    "(): unable to " + unableToWhat + "; ";
    return d;
}

//----------------------------------------------------------------

