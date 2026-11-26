//----------------------------------------------------------------
//
// File: OperationManager.cpp
//
//----------------------------------------------------------------

#include <cui/managers/OperationManager.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/dialogs/DialogError.h>
#include <cui/dialogs/DialogMessage.h>
#include <cui/operations/AutoOddsOperations.h>
#include <cui/operations/BetFlagsOperation.h>
#include <cui/operations/BetOperations.h>
#include <cui/operations/PlayerOperations.h>
#include <cui/operations/QuickBetsOperations.h>
#include <cui/operations/QuitOperation.h>
#include <cui/operations/SettingsOperations.h>
#include <cui/operations/TableOperations.h>
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
//
// Start a workflow. The previous Operation, if any, finished some time
// ago and was only kept alive until the hook that finished it returned.
// It is safe to drop it now.
//
bool
OperationManager::begin(std::unique_ptr<Operation> op)
{
    if (isActive()) return false;

    retired_.reset();
    current_ = std::move(op);
    current_->start();
    return true;
}

//----------------------------------------------------------------

bool
OperationManager::isActive() const
{
    return stage_ != Stage::Idle;
}

//----------------------------------------------------------------
//
// The workflows. Each one is an Operation (see cui/operations/).
//

void
OperationManager::makeBet()
{
    if (!begin(std::make_unique<MakeBetOperation>()))
    {
        // TODO can't just log if this happens
        LOG_WARN("OperationManager::makeBet(): operation already active");
    }
}

bool OperationManager::removeBets()
    { return begin(std::make_unique<RemoveBetOperation>()); }

bool OperationManager::undoLastBet()
    { return begin(std::make_unique<UndoBetOperation>()); }

bool OperationManager::betFlags(Ctrl::BetFlags::Scope scope)
    { return begin(std::make_unique<BetFlagsOperation>(scope)); }

bool OperationManager::applyQuickBets()
    { return begin(std::make_unique<ApplyQuickBetsOperation>()); }

bool OperationManager::quickBetsSetOrAdd()
    { return begin(std::make_unique<SaveToQuickBetsOperation>()); }

bool OperationManager::manageQuickBets()
    { return begin(std::make_unique<ManageQuickBetsOperation>()); }

bool OperationManager::manageAutoOdds()
    { return begin(std::make_unique<ConfigureAutoOddsOperation>()); }

bool OperationManager::autoOddsSetOrUnset()
    { return begin(std::make_unique<SetAutoOddsOperation>()); }

bool OperationManager::chooseBuddyFollow()
    { return begin(std::make_unique<BuddyFollowOperation>()); }

bool OperationManager::chooseAnimationSpeed()
    { return begin(std::make_unique<AnimationSpeedOperation>()); }

bool OperationManager::chooseRollHistoryFormat()
    { return begin(std::make_unique<RollHistoryFormatOperation>()); }

bool OperationManager::confirmQuit()
    { return begin(std::make_unique<QuitOperation>()); }

//----------------------------------------------------------------

bool
OperationManager::tableOperation(TableOp op)
{
    switch (op)
    {
    case TableOp::List:
        return begin(std::make_unique<ListTablesOperation>());
    case TableOp::Switch:
        return begin(std::make_unique<SwitchTableOperation>());
    }
    return false;
}

//----------------------------------------------------------------

bool
OperationManager::playerOperation(PlayerOp op)
{
    switch (op)
    {
    case PlayerOp::List:
        return begin(std::make_unique<ShowPlayersOperation>(false));
    case PlayerOp::ListBuddies:
        return begin(std::make_unique<ShowPlayersOperation>(true));
    case PlayerOp::Switch:
        return begin(std::make_unique<SwitchPlayerOperation>());
    case PlayerOp::Rename:
        return begin(std::make_unique<RenamePlayerOperation>());
    case PlayerOp::Create:
        return begin(std::make_unique<CreatePlayerOperation>());
    case PlayerOp::Delete:
        return begin(std::make_unique<DeletePlayerOperation>());
    case PlayerOp::BuddyJoins:
        return begin(std::make_unique<BuddyJoinsOperation>());
    case PlayerOp::BuddyLeaves:
        return begin(std::make_unique<BuddyLeavesOperation>());
    }
    return false;
}

//----------------------------------------------------------------
//
// What happened in a dialog. It goes to the Operation that pushed the
// dialog, if it is waiting for that kind of answer.
//

void
OperationManager::onPickResult(bool accepted, const Craps::PlayerId& playerId,
                               Craps::BetId betId, std::size_t tag)
{
    if (current_ && stage_ == Stage::AwaitingPick)
    {
        current_->onPick(accepted, playerId, betId, tag);
    }
}

void
OperationManager::onQuickBetsChoice(bool accepted, std::size_t index)
{
    if (current_ && stage_ == Stage::AwaitingPick)
    {
        current_->onQuickBetsChoice(accepted, index);
    }
}

void
OperationManager::onTextResult(bool accepted, const std::string& text)
{
    if (current_ && stage_ == Stage::AwaitingText)
    {
        current_->onText(accepted, text);
    }
}

void
OperationManager::onConfirmationResult(SurfaceBase::OperationResult result)
{
    if (current_ && stage_ == Stage::AwaitingConfirmation)
    {
        current_->onConfirm(result);
    }
}

void
OperationManager::onErrorAcknowledged()
{
    // DialogError has already popped itself.
    if (current_ && stage_ == Stage::AwaitingErrorAcknowledgement)
    {
        current_->onErrorAcknowledged();
    }
}

//----------------------------------------------------------------
//
// The Controller's answers. One that is not for the running Operation (a
// late answer, or one for another operation) is ignored.
//

void OperationManager::onRspMakeBet(const Ctrl::RspMakeBet& ev)
    { route("RspMakeBet", ev, &Operation::onRspMakeBet); }

void OperationManager::onRspOddsBet(const Ctrl::RspOddsBet& ev)
    { route("RspOddsBet", ev, &Operation::onRspOddsBet); }

void OperationManager::onRspRemoveBet(const Ctrl::RspRemoveBet& ev)
    { route("RspRemoveBet", ev, &Operation::onRspRemoveBet); }

void OperationManager::onRspUndoBet(const Ctrl::RspUndoBet& ev)
    { route("RspUndoBet", ev, &Operation::onRspUndoBet); }

void OperationManager::onRspSetBetFlags(const Ctrl::RspSetBetFlags& ev)
    { route("RspSetBetFlags", ev, &Operation::onRspSetBetFlags); }

void OperationManager::onRspApplyQuickBets(const Ctrl::RspApplyQuickBets& ev)
    { route("RspApplyQuickBets", ev, &Operation::onRspApplyQuickBets); }

void OperationManager::onRspEditQuickBets(const Ctrl::RspEditQuickBets& ev)
    { route("RspEditQuickBets", ev, &Operation::onRspEditQuickBets); }

void OperationManager::onRspEditAutoOdds(const Ctrl::RspEditAutoOdds& ev)
    { route("RspEditAutoOdds", ev, &Operation::onRspEditAutoOdds); }

void OperationManager::onRspSwitchPlayer(const Ctrl::RspSwitchPlayer& ev)
    { route("RspSwitchPlayer", ev, &Operation::onRspSwitchPlayer); }

void OperationManager::onRspManagePlayer(const Ctrl::RspManagePlayer& ev)
    { route("RspManagePlayer", ev, &Operation::onRspManagePlayer); }

void OperationManager::onRspSwitchTable(const Ctrl::RspSwitchTable& ev)
    { route("RspSwitchTable", ev, &Operation::onRspSwitchTable); }

//----------------------------------------------------------------

template <class Rsp>
void
OperationManager::route(const char* name, const Rsp& ev,
                        void (Operation::*hook)(const Rsp&))
{
    if (current_ && stage_ == Stage::AwaitingResponse &&
        ev.correlationId == correlationId_)
    {
        (current_.get()->*hook)(ev);
        return;
    }

    LOG_WARN(std::string("OperationManager: ignoring unmatched ") + name +
             ": rsp.correlationId = " + std::to_string(ev.correlationId) +
             ", have correlationId_ = " + std::to_string(correlationId_));
}

//----------------------------------------------------------------

void
OperationManager::finish()
{
    // The Operation that is finishing may be the one running right now.
    // Keep it until begin() starts the next one. (A second finish() must
    // not drop it while it is still running.)
    if (current_) retired_ = std::move(current_);

    stage_         = Stage::Idle;
    correlationId_ = 0;
}

//----------------------------------------------------------------
//
// What the screen shows: dialogs the Operations ask for
//

void
OperationManager::showError(const Gen::ErrorPass& error,
                            const char* defaultType,
                            const char* defaultName)
{
    LOG_TRACE("OperationManager::showError()");

    // TODO labels should have already been set
    Gen::ErrorPass displayError = error;
    if (displayError.errorTypeStr().empty())
        displayError.setErrorType(defaultType);
    if (displayError.errorNameStr().empty())
        displayError.setErrorName(defaultName);
    if (displayError.description().empty())
        displayError.setDescription("The requested operation could not be completed.");

    stage_ = Stage::AwaitingErrorAcknowledgement;
    LOG_TRACE("calling DialogError::instance().configure(error)");
    DialogError::instance().configure(displayError);
    SurfaceManager::instance().pushSurface(&DialogError::instance());
}

//----------------------------------------------------------------

void
OperationManager::showNotice(const std::string& brief,
                             const std::string& detail)
{
    DialogMessage::MessageText m;
    m.label  = DialogMessage::MessageLabel::Info;
    m.brief  = brief;
    m.detail = detail;
    DialogMessage::instance().configure(m);
    SurfaceManager::instance().pushSurface(&DialogMessage::instance());
}

//----------------------------------------------------------------
//
// AutoOdds acted after betting reopened. The status line says how many
// were placed; a notice lists what could not be, unless the player is in
// the middle of something else.
//
void
OperationManager::onAutoOddsApplied(const Ctrl::UslAutoOddsApplied& ev)
{
    LOG_TRACE("OperationManager::onAutoOddsApplied()");

    autoOddsStatus_ = "AutoOdds: " + std::to_string(ev.placed) + " placed";
    SurfaceManager::instance().draw();
    if (!ev.skippedDetail.empty() && !isActive())
    {
        showNotice(ev.headline, ev.skippedDetail);
    }
}

//----------------------------------------------------------------

void
OperationManager::clearAutoOddsStatus()
{
    autoOddsStatus_.clear();
}

//----------------------------------------------------------------

const std::string&
OperationManager::autoOddsStatus() const
{
    return autoOddsStatus_;
}
//----------------------------------------------------------------
