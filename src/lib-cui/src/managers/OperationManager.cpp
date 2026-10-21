//----------------------------------------------------------------
//
// File: OperationManager.cpp
//
//----------------------------------------------------------------

#include <cui/managers/OperationManager.h>
#include <cui/core/CuiSender.h>
#include <cui/dialogs/DialogBetAmount.h>
#include <cui/dialogs/DialogConfirm.h>
#include <cui/dialogs/DialogError.h>
#include <cui/managers/CarrierBet.h>
#include <cui/managers/SurfaceManager.h>
#include <gen/Logger.h>
#include <string>

using namespace Cui;

//----------------------------------------------------------------

OperationManager&
OperationManager::instance()
{
    static OperationManager mgr;
    return mgr;
}

//----------------------------------------------------------------

bool
OperationManager::isActive() const
{
    return stage_ != Stage::Idle;
}

//----------------------------------------------------------------

void
OperationManager::makeBet()
{
    if (isActive())
    {
        // TODO can't just log if this happens
        LOG_WARN("OperationManager::makeBet(): operation already active");
        return;
    }
    
    type_          = OperationType::MakeBet;
    stage_         = Stage::AwaitingResponse;
    const auto& cb = CarrierBet::instance();
    
    if (cb.isOddsBet())
    {
        doOddsBet(cb);
    }
    else
    {
        doContractBet(cb);
    }
}

//----------------------------------------------------------------

void
OperationManager::doContractBet(const CarrierBet& cb)
{
    correlationId_ = CuiSender::instance().reqMakeBet(
                         cb.getPlayerId(),
                         cb.getBetName(),
                         cb.getContractAmount(),
                         cb.getPivot());
    LOG_TRACE("OperationManager::doContractBet(): request correlationId = " +
              std::to_string(correlationId_));
}

//----------------------------------------------------------------

void
OperationManager::doOddsBet(const CarrierBet& cb)
{
    correlationId_ = CuiSender::instance().reqOddsBet(
                         cb.getPlayerId(),
                         cb.getBetId(),
                         cb.getOddsAmount());

    LOG_TRACE("OperationManager::doOddsBet(): request correlationId = " +
              std::to_string(correlationId_));
}

//----------------------------------------------------------------

void
OperationManager::onMakeBetResponse(const Ctrl::RspMakeBet& ev)
{
    LOG_TRACE("OperationManager::onMakeBetResponse()");
             
    if (stage_ != Stage::AwaitingResponse ||
        type_  != OperationType::MakeBet  ||
        ev.correlationId != correlationId_)
    {
        LOG_WARN("OperationManager::onMakeBetResponse(): ignoring "
                 "unmatched RspMakeBet: rsp.correlationId = "       +
                 std::to_string(ev.correlationId)                   +
                 ", have correlationId_ = "                         +
                 std::to_string(correlationId_));
        return;
    }

    if (ev.accepted)
    {
        // ReqMakeBet has succeeded. Unwind the active betting workflow.
        SurfaceManager::instance().popToRoot();
        finish();
        return;
    }

    showError(ev.error);
}

//----------------------------------------------------------------

void
OperationManager::showError(const Gen::ErrorPass& error)
{
    LOG_TRACE("OperationManager::showError()");

    // TODO labels should have already been set
    Gen::ErrorPass displayError = error;
    if (displayError.errorTypeStr().empty())
        displayError.setErrorType("Betting Error");
    if (displayError.errorNameStr().empty())
        displayError.setErrorName("Unable to Make Bet");
    if (displayError.description().empty())
        displayError.setDescription("The requested operation could not be completed.");

    stage_ = Stage::AwaitingErrorAcknowledgement;
    LOG_TRACE("calling DialogError::instance().configure(error)");
    DialogError::instance().configure(displayError);
    SurfaceManager::instance().pushSurface(&DialogError::instance());
}

//----------------------------------------------------------------

void
OperationManager::onErrorAcknowledged()
{
    if (stage_ != Stage::AwaitingErrorAcknowledgement) return;

    // DialogError has already popped itself. Now unwind the operation's
    // menus/dialogs to the root screen and release the operation state.
    SurfaceManager::instance().popToRoot();
    finish();
}

//----------------------------------------------------------------

bool
OperationManager::confirmQuit()
{
    if (isActive()) return false;

    type_ = OperationType::Quit;
    stage_ = Stage::AwaitingConfirmation;
    DialogConfirm::instance().configure("Confirm Quit RoyalCraps game");
    SurfaceManager::instance().pushSurface(&DialogConfirm::instance());
    return true;
}

//----------------------------------------------------------------

void
OperationManager::onConfirmationResult(SurfaceBase::OperationResult result)
{
    if (stage_ != Stage::AwaitingConfirmation || type_ != OperationType::Quit)
        return;

    if (result == SurfaceBase::OperationResult::Yes)
    {
        CuiSender::instance().reqTerminate();
    }
    finish();
}

//----------------------------------------------------------------

void
OperationManager::finish()
{
    type_          = OperationType::None;
    stage_         = Stage::Idle;
    correlationId_ = 0;
}

//----------------------------------------------------------------
