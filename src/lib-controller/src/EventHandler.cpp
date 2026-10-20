//----------------------------------------------------------------
//
// File: EventHandler.cpp
//
//----------------------------------------------------------------

#include <controller/EventHandler.h>
#include <controller/GameEvents.h>
#include <controller/GameMain.h>
#include <controller/Globals.h>
#include <controller/AutoFill.h>
#include <controller/QuickBet.h>
#include <craps/CrapsTable.h>
#include <gen/EventManager.h>
#include <gen/Logger.h>

using namespace Ctrl;

//----------------------------------------------------------------

EventHandler::EventHandler()
{
    Gen::EventManager::instance().subscribe<Ctrl::ReqRollDice>(
        [this](const Ctrl::ReqRollDice& ev)
        {
            this->onReqRollDice(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::ReqAutoFill>(
        [this](const Ctrl::ReqAutoFill& ev)
        {
            this->onReqAutoFill(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::ReqQuickBet>(
        [this](const Ctrl::ReqQuickBet& ev)
        {
            this->onReqQuickBet(ev);
        });

    Gen::EventManager::instance().subscribe<Ctrl::ReqTerminate>(
        [this](const Ctrl::ReqTerminate& ev)
        {
            this->onReqTerminate(ev);
        });
}

//----------------------------------------------------------------
//
//  Events from the View (lib-cui, lib-gui)
//
//----------------------------------------------------------------

void
EventHandler::onReqRollDice(const Ctrl::ReqRollDice& ev) const
{
    LOG_TRACE("Entered Controller::EventHandler::onReqRollDice()");
    Gbl::pTable->rollDice();
    
    // No response event
}

//----------------------------------------------------------------

void
EventHandler::onReqAutoFill(const Ctrl::ReqAutoFill& ev) const
{
    LOG_TRACE("Entered Controller::EventHandler::onReqAutoFill()");
    
    AutoFill::AutoFillEntry afe
    {
        ev.betName,
        ev.pivot,
        ev.isOddsBet,
        ev.amount
    };
    AutoFill::instance().setAutoFill(afe);

    // No response event
}

//----------------------------------------------------------------

void
EventHandler::onReqQuickBet(const Ctrl::ReqQuickBet& ev) const
{
    LOG_TRACE("Entered Controller::EventHandler::onReqQuickBet()");

    QuickBet::QuickBetEntry qbe
    {
        ev.betName,
        ev.pivot,
        ev.isOddsBet,
        ev.amount
    };
    QuickBet::instance().setQuickBet(qbe);

    // No response event
}

//----------------------------------------------------------------

void
EventHandler::onReqTerminate(const Ctrl::ReqTerminate& ev) const
{
    LOG_TRACE("Entered Controller::EventHandler::onReqTerminate()");
    
    GameMain::instance()->terminateApp();

    // No response event
}

//----------------------------------------------------------------
//
//  Events from the Model (lib-craps) we're interested in handling
//  here in the Controller.
//
//----------------------------------------------------------------

//----------------------------------------------------------------

