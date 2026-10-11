//----------------------------------------------------------------
//
// File: EventHandler.cpp
//
//----------------------------------------------------------------

#include <controller/EventHandler.h>
#include <controller/GameEvents.h>
#include <controller/GameMain.h>
#include <controller/Globals.h>
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
    LOG_TRACE("Entered EventHandler::onReqRollDice()");
    Gbl::pTable->rollDice();
}

//----------------------------------------------------------------

void
EventHandler::onReqTerminate(const Ctrl::ReqTerminate& ev) const
{
    LOG_TRACE("Entered EventHandler::onReqTerminate()");
    GameMain::instance()->terminateApp();
}

//----------------------------------------------------------------
//
//  Events from the Model (lib-craps) we're interested in handling
//  here in the Controller.
//
//----------------------------------------------------------------

//----------------------------------------------------------------

