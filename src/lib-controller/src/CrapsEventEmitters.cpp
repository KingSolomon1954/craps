//----------------------------------------------------------------
//
// File: CrapsEventEmitters.cpp
//
//----------------------------------------------------------------

#include <controller/CrapsEventEmitters.h>
#include <controller/AutoFill.h>
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

Gen::ReturnCode
CrapsEventEmitters::playerJoinTable(
    const Craps::PlayerId& playerId,
    const Craps::TableId& tableId,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
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
CrapsEventEmitters::playerLeaveTable(
    const Craps::PlayerId& playerId,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
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
CrapsEventEmitters::playerMakeBet(
    const Craps::PlayerId& playerId,
    BetName betName,
    Gen::Money contractAmount,
    unsigned pivot,
    Craps::BetId& betId,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
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
//
// Bet related
//
//----------------------------------------------------------------

Gen::ReturnCode
CrapsEventEmitters::betSetContractAmount(
    const Craps::BetId& betId,
    Gen::Money contractAmount,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betSetContractAmount", "setContractAmount"));
        return Gen::ReturnCode::Fail;
    }

    auto rc = pBet->player().setContractAmount(betId, contractAmount, ep);
    if (rc == Gen::ReturnCode::Success)
    {
        Gbl::pUndoMgr->push(std::make_unique<UndoBetModifiedAmount>(pBet));
    }
    return rc;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsEventEmitters::betSetOddsAmount(
    const Craps::BetId& betId,
    Gen::Money oddsAmount,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
    auto pBet = Gbl::pTable->getBet(betId, ep);
    if (pBet == nullptr)
    {
        ep.prepend(diagPrefix("betSetOddsAmount", "setOddsAmount"));
        return Gen::ReturnCode::Fail;
    }
    auto rc = pBet->player().setOddsAmount(pBet, oddsAmount, ep);
    if (rc == Gen::ReturnCode::Success)
    {
        Gbl::pUndoMgr->push(std::make_unique<UndoBetModifiedAmount>(pBet));
    }
    return rc;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsEventEmitters::betRemove(
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

Gen::ReturnCode
CrapsEventEmitters::betSetOffComeOutRoll(
    const Craps::BetId& betId,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
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
CrapsEventEmitters::betSetOnComeOutRoll(
    const Craps::BetId& betId,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
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
CrapsEventEmitters::betSetHardwayOff(
    const Craps::BetId& betId,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
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
CrapsEventEmitters::betSetHardwayOn(
    const Craps::BetId& betId,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
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
CrapsEventEmitters::rollDice(
    const Craps::TableId& tableId,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
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
//
// Auto Fills
//
//----------------------------------------------------------------

Gen::ReturnCode
CrapsEventEmitters::setAutoFill(
    const AutoFillEntry& entry,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
    AutoFill::AutoFillEntry afe =
        {entry.betName, entry.pivot, entry.oddsBet, entry.amount };
    AutoFill::instance()->setAutoFill(afe);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsEventEmitters::deleteAutoFill(
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

Gen::ReturnCode
CrapsEventEmitters::setQuickBet(
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

Gen::ReturnCode
CrapsEventEmitters::applyQuickBet(
    size_t index,
    Gen::ErrorPass& ep)
{
    // TODO turn into event
    QuickBet::instance()->applyQuickBet(index);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

Gen::ReturnCode
CrapsEventEmitters::deleteQuickBet(
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

Gen::ReturnCode
CrapsEventEmitters::deleteQuickBet(
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

Gen::ReturnCode
CrapsEventEmitters::undoLast(
    Gen::ErrorPass& ep)
{
    // TODO turn into event
    if (Gbl::pUndoMgr->canUndo())
    {
        Gbl::pUndoMgr->undo();
        return Gen::ReturnCode::Success;
    }
    ep.diag = "CrapsEventEmitters::betUndoLast(): Nothing to undo. "
              "Undo stack is empty.";
    return Gen::ReturnCode::Fail;
}

//----------------------------------------------------------------

void
CrapsEventEmitters::gameTerminate()
{
    // TODO
}

//----------------------------------------------------------------

// Private helper
std::string
CrapsEventEmitters::diagPrefix(
    const std::string& funcName,
    const std::string& unableToWhat)
{
    std::string d = "CrapsEventEmitters::" + funcName + 
                    "(): unable to " + unableToWhat + "; ";
    return d;
}

//----------------------------------------------------------------

