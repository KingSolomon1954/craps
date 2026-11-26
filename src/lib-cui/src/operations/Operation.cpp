//----------------------------------------------------------------
//
// File: Operation.cpp
//
//----------------------------------------------------------------

#include <cui/operations/Operation.h>
#include <cui/managers/OperationManager.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/dialogs/DialogConfirm.h>
#include <gen/Logger.h>

using namespace Cui;

//----------------------------------------------------------------
//
// The hooks an Operation does not override. A dialog answered that the
// workflow did not ask for is a mistake in the code, not in the person's
// input. Say so and end the workflow, so the person is not left stuck.
//

void
Operation::onPick(bool, const Craps::PlayerId&, Craps::BetId, std::size_t)
{
    LOG_WARN("Operation::onPick(): unexpected pick result");
    finish();
}

//----------------------------------------------------------------

void
Operation::onText(bool, const std::string&)
{
    LOG_WARN("Operation::onText(): unexpected text result");
    finish();
}

//----------------------------------------------------------------

void
Operation::onQuickBetsChoice(bool, std::size_t)
{
    LOG_WARN("Operation::onQuickBetsChoice(): unexpected choice");
    finish();
}

//----------------------------------------------------------------

void
Operation::onConfirm(SurfaceBase::OperationResult)
{
    LOG_WARN("Operation::onConfirm(): unexpected confirmation result");
    finish();
}

//----------------------------------------------------------------

// A response nobody asked this Operation for. Ignore it.
#define UNEXPECTED_RESPONSE(Type, name)                                  \
void                                                                     \
Operation::name(const Ctrl::Type&)                                       \
{                                                                        \
    LOG_WARN("Operation::" #name "(): unexpected response, ignored");    \
}

UNEXPECTED_RESPONSE(RspMakeBet,     onRspMakeBet)
UNEXPECTED_RESPONSE(RspOddsBet,     onRspOddsBet)
UNEXPECTED_RESPONSE(RspRemoveBet,   onRspRemoveBet)
UNEXPECTED_RESPONSE(RspUndoBet,     onRspUndoBet)
UNEXPECTED_RESPONSE(RspSetBetFlags, onRspSetBetFlags)
UNEXPECTED_RESPONSE(RspApplyQuickBets, onRspApplyQuickBets)
UNEXPECTED_RESPONSE(RspEditQuickBets,  onRspEditQuickBets)
UNEXPECTED_RESPONSE(RspEditAutoOdds,   onRspEditAutoOdds)
UNEXPECTED_RESPONSE(RspSwitchPlayer,   onRspSwitchPlayer)
UNEXPECTED_RESPONSE(RspManagePlayer,   onRspManagePlayer)
UNEXPECTED_RESPONSE(RspSwitchTable,    onRspSwitchTable)

#undef UNEXPECTED_RESPONSE

//----------------------------------------------------------------

void
Operation::onErrorAcknowledged()
{
    // DialogError has already popped itself. Unwind the workflow's menus
    // and dialogs to the root screen.
    SurfaceManager::instance().popToRoot();
    finish();
}

//----------------------------------------------------------------

void
Operation::finish()
{
    OperationManager::instance().finish();
}

//----------------------------------------------------------------

void
Operation::awaitResponse(Ctrl::CorrelationId id)
{
    auto& mgr = OperationManager::instance();
    mgr.stage_         = OperationManager::Stage::AwaitingResponse;
    mgr.correlationId_ = id;
}

//----------------------------------------------------------------

void
Operation::showError(const Gen::ErrorPass& error,
                     const char* defaultType,
                     const char* defaultName)
{
    OperationManager::instance().showError(error, defaultType, defaultName);
}

//----------------------------------------------------------------

void
Operation::showNotice(const std::string& brief, const std::string& detail)
{
    OperationManager::instance().showNotice(brief, detail);
}

//----------------------------------------------------------------

void
Operation::finishAtRoot()
{
    SurfaceManager::instance().popToRoot();
    finish();
}

//----------------------------------------------------------------

void
Operation::pickFrom(const std::string& title,
                    std::vector<DialogPicker::Item> items,
                    DialogPicker::Mode mode)
{
    OperationManager::instance().stage_ = OperationManager::Stage::AwaitingPick;
    DialogPicker::instance().configure(title, std::move(items), mode);
    SurfaceManager::instance().pushSurface(&DialogPicker::instance());
}

//----------------------------------------------------------------

void
Operation::askText(const std::string& title,
                   const std::string& prompt,
                   const std::string& initial,
                   std::size_t maxChars,
                   DialogTextInput::Validator validator)
{
    OperationManager::instance().stage_ = OperationManager::Stage::AwaitingText;
    DialogTextInput::instance().configure(title, prompt, initial, maxChars,
                                          std::move(validator));
    SurfaceManager::instance().pushSurface(&DialogTextInput::instance());
}

//----------------------------------------------------------------

void
Operation::awaitPick()
{
    OperationManager::instance().stage_ = OperationManager::Stage::AwaitingPick;
}

//----------------------------------------------------------------

void
Operation::askConfirmation(const std::string& question)
{
    OperationManager::instance().stage_ =
        OperationManager::Stage::AwaitingConfirmation;
    DialogConfirm::instance().configure(question);
    SurfaceManager::instance().pushSurface(&DialogConfirm::instance());
}

//----------------------------------------------------------------
