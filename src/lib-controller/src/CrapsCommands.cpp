//----------------------------------------------------------------
//
// File: CrapsCommands.cpp
//
//----------------------------------------------------------------

#include <controller/CrapsCommands.h>
#include <controller/AutoFill.h>
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
    auto ev = std::make_shared<CmdPlayerJoinTable>();
    ev->correlationId = Gbl::pGameCtrl->nextCorrelationId();
    ev->playerId      = playerId;
    ev->tableId       = tableId;

    Gbl::pGameCtrl->enqueue(ev);

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
    auto ev = std::make_shared<CmdPlayerLeaveTable>();
    ev->correlationId = Gbl::pGameCtrl->nextCorrelationId();
    ev->playerId      = playerId;

    Gbl::pGameCtrl->enqueue(ev);

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
    auto ev = std::make_shared<CmdMakeBet>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->playerId       = playerId;
    ev->betName        = betName;
    ev->contractAmount = contractAmount;
    ev->pivot          = pivot;

    Gbl::pGameCtrl->enqueue(ev);

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
    auto ev = std::make_shared<CmdMakeBetAuto>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->playerId       = playerId;
    ev->betName        = betName;
    ev->contractAmount = contractAmount;
    ev->pivot          = pivot;

    Gbl::pGameCtrl->enqueue(ev);

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
    auto ev = std::make_shared<CmdBetSetContractAmount>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->betId          = betId;
    ev->contractAmount = contractAmount;

    Gbl::pGameCtrl->enqueue(ev);

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
    auto ev = std::make_shared<CmdBetSetOddsAmount>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->betId          = betId;
    ev->oddsAmount     = oddsAmount;

    Gbl::pGameCtrl->enqueue(ev);

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
    auto ev = std::make_shared<CmdBetSetOffComeOutRoll>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->betId          = betId;

    Gbl::pGameCtrl->enqueue(ev);

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
    auto ev = std::make_shared<CmdBetSetOnComeOutRoll>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->betId          = betId;

    Gbl::pGameCtrl->enqueue(ev);

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
    auto ev = std::make_shared<CmdBetSetHardwayOff>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->betId          = betId;

    Gbl::pGameCtrl->enqueue(ev);

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
    auto ev = std::make_shared<CmdBetSetHardwayOn>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->betId          = betId;

    Gbl::pGameCtrl->enqueue(ev);

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
    auto ev = std::make_shared<CmdRollDice>();
    ev->correlationId  = Gbl::pGameCtrl->nextCorrelationId();
    ev->tableId        = tableId;

    Gbl::pGameCtrl->enqueue(ev);

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
Gen::ReturnCode
CrapsCommands::cmdSetAutoFill(
    const AutoFillEntry& entry,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
    AutoFill::AutoFillEntry afe =
        {entry.betName, entry.pivot, entry.oddsBet, entry.amount};
    AutoFill::instance()->setAutoFill(afe);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------
//
// UI - user is deleting auto fill for a bet
//
Gen::ReturnCode
CrapsCommands::cmdDeleteAutoFill(
    const AutoFillEntry& entry,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
    AutoFill::AutoFillEntry afe =
        {entry.betName, entry.pivot, entry.oddsBet, entry.amount };
    AutoFill::instance()->deleteAutoFill(afe);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------
//
// Quick Bets
//
//----------------------------------------------------------------
//
// UI - user is configuring a quick bet
//
Gen::ReturnCode
CrapsCommands::cmdSetQuickBet(
    const QuickBetEntry& entry,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
    QuickBet::QuickBetEntry qbe =
        {entry.betName, entry.pivot, entry.oddsBet, entry.amount };
    QuickBet::instance()->setQuickBet(qbe);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------
//
// UI - user is applying a quick bet
//
Gen::ReturnCode
CrapsCommands::cmdApplyQuickBet(
    size_t index,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
    QuickBet::instance()->applyQuickBet(index);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------
//
// UI - user is deleting a quick bet (looking it up)
//
Gen::ReturnCode
CrapsCommands::cmdDeleteQuickBet(
    const QuickBetEntry& entry,
    BetName betName,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
    QuickBet::QuickBetEntry qbe =
        {entry.betName, entry.pivot, entry.oddsBet, entry.amount };
    QuickBet::instance()->deleteQuickBet(qbe);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------
//
// UI - user is deleting a quick bet (index already in hand)
//
Gen::ReturnCode
CrapsCommands::cmdDeleteQuickBet(
    size_t index,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
    QuickBet::instance()->deleteQuickBet(index);
    return Gen::ReturnCode::Success;
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

