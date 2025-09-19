//----------------------------------------------------------------
//
// File: CrapsEventHandlers.cpp
//
//----------------------------------------------------------------

#include <controller/CrapsEventHandlers.h>

using namespace Ctrl;

//----------------------------------------------------------------
//
// Tell model to make a bet, convert event
//
void
CrapsEventHandlers::onUserMakeBet(GameEvent* pBase)
{
    // TODO
    // auto rc = model->playerMakeBet()
    
    // if rc == success
    //     ViewEventEmmitters::emitMakeBetSuccess(pBase->correlationId)
    // else
    //     ViewEventEmmitters::emitDialogError(pBase->correlationId)
}
    
//----------------------------------------------------------------
//
// Tell model to make an odds bet, convert event
//
void
CrapsEventHandlers::onUserMakeOddsBet(GameEvent* pBase)
{
    // TODO
    // auto rc = model->playerMakeOddsBet()
    
    // if rc == success
    //     ViewEventEmmitters::emitMakeOddsBetSuccess(pBase->correlationId)
    // else
    //     ViewEventEmmitters::emitDialogError(pBase->correlationId)
}
    
//----------------------------------------------------------------
//
// Tell model to roll dice, convert event
//
void
CrapsEventHandlers::onUserRollDice(GameEvent* pBase)
{
    // TODO
    // auto rc = Gbl::pTable->rollDice()
}
    
//----------------------------------------------------------------
