//----------------------------------------------------------------
//
// File: CuiView.cpp
//
//----------------------------------------------------------------

#include <cui/CuiView.h>
#include <controller/Globals.h>
#include <controller/GameEvent.h>
#include <controller/GameController.h>
#include <craps/CrapsTypes.h>
#include <gen/Logger.h>

using namespace Cui;

//----------------------------------------------------------------

CuiView&
CuiView::instance()
{
    static CuiView v;
    return v;
}

//----------------------------------------------------------------

void
CuiView::showSuccess(
    Ctrl::EventType orgEventType,
    uint64_t correlationId)
{
    // TODO
}

//----------------------------------------------------------------
//
// Show Error Dialog. Overrides ViewInterface.
//
void
CuiView::showErrorDialog(
    Ctrl::EventType    orgEventType,
    uint64_t           correlationId,
    const std::string& diag)
{
    // Inform all UI listeners
    // Notify menus/screens of an error, puts up an ErrorDialog
    // uiEvent.correlationId = ev->correlationId;
    // uiEvent.orgEventType = ev->orgEventType;
    // uiEvent.diag = ev->diag;
    // uiEventMgr->publish(uiEvent);
}

//----------------------------------------------------------------

void
CuiView::showMakeBetSuccess(
    Craps::BetId betId,
    uint64_t correlationId)
{
    // TODO
}

//----------------------------------------------------------------

void
CuiView::showMakeOddsBetSuccess(
    Craps::BetId betId,
    uint64_t correlationId)
{
    // TODO
}

//----------------------------------------------------------------

void
CuiView::showMakeBetAutoSuccess(
    Craps::BetId betId,
    uint64_t correlationId)
{
    // TODO
}

//----------------------------------------------------------------

void
CuiView::showMakeBetAutoError(
    Ctrl::EventType orgEventType,
    uint64_t correlationId,
    const Craps::PlayerId& playerId,
    const std::string& diag)
{
    // TODO
}

//----------------------------------------------------------------

void
CuiView::showRollDiceCountDown(
    uint64_t correlationId,
    int      numSeconds)
{
    // TODO
}

//----------------------------------------------------------------

void
CuiView::showRollDiceAnimation(uint64_t correlationId)
{
    // TODO
}

//----------------------------------------------------------------

void
CuiView::showProgramExit()
{
    // TODO
}

//----------------------------------------------------------------
