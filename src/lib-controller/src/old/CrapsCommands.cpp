//----------------------------------------------------------------
//
// File: CrapsCommands.cpp
//
//----------------------------------------------------------------

#include <controller/CrapsCommands.h>
#include <controller/GameController.h>
#include <controller/GameEvent.h>
#include <controller/Globals.h>
#include <controller/PlayerManager.h>
#include <controller/QuickBet.h>
#include <controller/TableManager.h>
#include <controller/UndoManager.h>
#include <craps/CrapsTable.h>
#include <craps/Player.h>

using namespace Ctrl;

//----------------------------------------------------------------
//
// Player related
//
//----------------------------------------------------------------
//
// User pressed menu for player to join table
//
uint64_t
CrapsCommands::cmdPlayerJoinTable(
    const Craps::PlayerId& playerId,
    const Craps::TableId&  tableId)
{
    auto ev = std::make_unique<CmdPlayerJoinTable>();
    ev->correlationId = Gbl::pGameCtrl->nextCorrelationId();
    ev->playerId      = playerId;
    ev->tableId       = tableId;

    Gbl::pGameCtrl->enqueue(std::move(ev));

    return ev->correlationId;
}

//----------------------------------------------------------------
//
// User pressed menu for player to leave table
//
uint64_t
CrapsCommands::cmdPlayerLeaveTable(
    const Craps::PlayerId& playerId)
{
    auto ev = std::make_unique<CmdPlayerLeaveTable>();
    ev->correlationId = Gbl::pGameCtrl->nextCorrelationId();
    ev->playerId      = playerId;

    Gbl::pGameCtrl->enqueue(std::move(ev));

    return ev->correlationId;
}

//----------------------------------------------------------------
//
// UI - user making a bet
//
uint64_t
CrapsCommands::cmdMakeBet(
    const Craps::PlayerId& playerId,
    BetName betName,
    Gen::Money contractAmount,
    size_t pivot)
{
    auto ev = std::make_unique<CmdMakeBet>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->playerId       = playerId;
    ev->betName        = betName;
    ev->contractAmount = contractAmount;
    ev->pivot          = pivot;

    Gbl::pGameCtrl->enqueue(std::move(ev));

    return ev->correlationId;
}
    
//----------------------------------------------------------------
//
// Automation entry point - player making a bet
//
uint64_t
CrapsCommands::cmdMakeBetAuto(
    const Craps::PlayerId& playerId,
    BetName betName,
    Gen::Money contractAmount,
    size_t pivot)
{
    auto ev = std::make_unique<CmdMakeBetAuto>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->playerId       = playerId;
    ev->betName        = betName;
    ev->contractAmount = contractAmount;
    ev->pivot          = pivot;

    Gbl::pGameCtrl->enqueue(std::move(ev));

    return ev->correlationId;
}
    
//----------------------------------------------------------------
//
// Bet related
//
//----------------------------------------------------------------
//
// UI - user changing/setting contract bet amount
//
uint64_t
CrapsCommands::cmdBetSetContractAmount(
    const Craps::BetId& betId,
    Gen::Money contractAmount)
{
    auto ev = std::make_unique<CmdBetSetContractAmount>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->betId          = betId;
    ev->contractAmount = contractAmount;

    Gbl::pGameCtrl->enqueue(std::move(ev));

    return ev->correlationId;
}

//----------------------------------------------------------------
//
// UI - user changing/setting odds amount on bet
//
uint64_t
CrapsCommands::cmdBetSetOddsAmount(
    const Craps::BetId& betId,
    Gen::Money oddsAmount)
{
    auto ev = std::make_unique<CmdBetSetOddsAmount>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->betId          = betId;
    ev->oddsAmount     = oddsAmount;

    Gbl::pGameCtrl->enqueue(std::move(ev));

    return ev->correlationId;
}

//----------------------------------------------------------------
//
// UI - user setting a bet flag
//
uint64_t
CrapsCommands::cmdBetSetOffComeOutRoll(
    const Craps::BetId& betId)
{
    auto ev = std::make_unique<CmdBetSetOffComeOutRoll>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->betId          = betId;

    Gbl::pGameCtrl->enqueue(std::move(ev));

    return ev->correlationId;
}

//----------------------------------------------------------------
//
// UI - user setting a bet flag
//
uint64_t
CrapsCommands::cmdBetSetOnComeOutRoll(
    const Craps::BetId& betId)
{
    auto ev = std::make_unique<CmdBetSetOnComeOutRoll>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->betId          = betId;

    Gbl::pGameCtrl->enqueue(std::move(ev));

    return ev->correlationId;
}

//----------------------------------------------------------------
//
// UI - user setting a bet flag
//
uint64_t
CrapsCommands::cmdBetSetHardwayOff(
    const Craps::BetId& betId)
{
    auto ev = std::make_unique<CmdBetSetHardwayOff>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->betId          = betId;

    Gbl::pGameCtrl->enqueue(std::move(ev));

    return ev->correlationId;
}

//----------------------------------------------------------------
//
// UI - user setting a bet flag
//
uint64_t
CrapsCommands::cmdBetSetHardwayOn(
    const Craps::BetId& betId)
{
    auto ev = std::make_unique<CmdBetSetHardwayOn>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->betId          = betId;

    Gbl::pGameCtrl->enqueue(std::move(ev));

    return ev->correlationId;
}

//----------------------------------------------------------------
//
// UI - user wants to remove a bet
//
Gen::ReturnCode
CrapsCommands::cmdBetRemove(
    const Craps::BetId& betId,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betRemove", "remove bet"));
        return Gen::ReturnCode::Fail;
    }

    auto rc = pBet->player().removeBet(betId, ep);
    if (rc == Gen::ReturnCode::Success)
    {
        Gbl::pUndoMgr->push(std::make_unique<UndoBetRemoved>(pBet));
    }
    return rc;
}

//----------------------------------------------------------------
//
// Table related
//
//----------------------------------------------------------------
//
// UI - user wants to roll dice
//
uint64_t
CrapsCommands::cmdRollDice(
    const Craps::TableId& tableId)
{
    auto ev = std::make_unique<CmdRollDice>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->tableId        = tableId;

    Gbl::pGameCtrl->enqueue(std::move(ev));

    return ev->correlationId;
}

//----------------------------------------------------------------
//
// Auto Fills
//
//----------------------------------------------------------------
//
// UI - user is configuring auto fill for a bet
//
uint64_t
CrapsCommands::cmdSetAutoFill(
    BetName    betName,
    size_t     pivot,
    bool       oddsBet,
    Gen::Money amount)
{
    auto ev = std::make_unique<CmdSetAutoFill>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->betName        = betName;
    ev->pivot          = pivot;
    ev->oddsBet        = oddsBet;
    ev->amount         = amount;

    Gbl::pGameCtrl->enqueue(std::move(ev));

    return ev->correlationId;
}

//----------------------------------------------------------------
//
// UI - user is deleting auto fill for a bet
//
uint64_t
CrapsCommands::cmdDeleteAutoFill(
    BetName    betName,
    size_t     pivot,
    bool       oddsBet,
    Gen::Money amount)
{
    auto ev = std::make_unique<CmdDeleteAutoFill>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->betName        = betName;
    ev->pivot          = pivot;
    ev->oddsBet        = oddsBet;
    ev->amount         = amount;

    Gbl::pGameCtrl->enqueue(std::move(ev));

    return ev->correlationId;
}

//----------------------------------------------------------------
//
// Quick Bets
//
//----------------------------------------------------------------
//
// UI - user is configuring a quick bet
//
uint64_t
CrapsCommands::cmdSetQuickBet(
        BetName    betName,
        size_t     pivot,
        bool       oddsBet,
        Gen::Money amount)
{
    auto ev = std::make_unique<CmdSetQuickBet>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->betName        = betName;
    ev->pivot          = pivot;
    ev->oddsBet        = oddsBet;
    ev->amount         = amount;

    Gbl::pGameCtrl->enqueue(std::move(ev));

    return ev->correlationId;
}

//----------------------------------------------------------------
//
// UI - user is applying a quick bet
//
uint64_t
CrapsCommands::cmdApplyQuickBet(
    size_t index)
{
    auto ev = std::make_unique<CmdApplyQuickBet>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->index          = index;

    Gbl::pGameCtrl->enqueue(std::move(ev));

    return ev->correlationId;
}

//----------------------------------------------------------------
//
// UI - user is deleting a quick bet (looking it up)
//
uint64_t
CrapsCommands::cmdDeleteQuickBetByLookup(
    BetName    betName,
    size_t     pivot,
    bool       oddsBet,
    Gen::Money amount)
{
    auto ev = std::make_unique<CmdDeleteQuickBetByLookup>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->betName        = betName;
    ev->pivot          = pivot;
    ev->oddsBet        = oddsBet;
    ev->amount         = amount;

    Gbl::pGameCtrl->enqueue(std::move(ev));

    return ev->correlationId;
}

//----------------------------------------------------------------
//
// UI - user is deleting a quick bet (index already in hand)
//
uint64_t
CrapsCommands::cmdDeleteQuickBetByIndex(
    size_t index)
{
    auto ev = std::make_unique<CmdDeleteQuickBetByIndex>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->index          = index;

    Gbl::pGameCtrl->enqueue(std::move(ev));

    return ev->correlationId;
}

//----------------------------------------------------------------
//
// Program related
//
//----------------------------------------------------------------
//
// UI - user wants to undo last bet
//
Gen::ReturnCode
CrapsCommands::undoLastBet(
    Gen::ErrorPass& ep)
{
    // TODO turn into event
    if (Gbl::pUndoMgr->canUndo())
    {
        Gbl::pUndoMgr->undo();
        return Gen::ReturnCode::Success;
    }
    ep.diag = "CrapsCommands::betUndoLast(): Nothing to undo. "
              "Undo stack is empty.";
    return Gen::ReturnCode::Fail;
}

//----------------------------------------------------------------

void
CrapsCommands::gameTerminate()
{
    // TODO
}

//----------------------------------------------------------------

// Private helper
std::string
CrapsCommands::diagPrefix(
    const std::string& funcName,
    const std::string& unableToWhat)
{
    std::string d = "CrapsCommands::" + funcName + 
                    "(): unable to " + unableToWhat + "; ";
    return d;
}

//----------------------------------------------------------------

