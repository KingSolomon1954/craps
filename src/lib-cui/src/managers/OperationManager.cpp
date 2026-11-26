//----------------------------------------------------------------
//
// File: OperationManager.cpp
//
//----------------------------------------------------------------

#include <cui/managers/OperationManager.h>
#include <cui/operations/BetFlagsOperation.h>
#include <cui/operations/BetOperations.h>
#include <cui/operations/AutoOddsOperations.h>
#include <cui/operations/OperationText.h>
#include <cui/operations/QuickBetsOperations.h>
#include <cui/operations/QuitOperation.h>
#include <cui/operations/SettingsOperations.h>
#include <cui/managers/CarrierBet.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/core/CuiSender.h>
#include <cui/dialogs/DialogBetAmount.h>
#include <cui/dialogs/DialogConfirm.h>
#include <cui/dialogs/DialogError.h>
#include <cui/dialogs/DialogMessage.h>
#include <cui/dialogs/DialogPicker.h>
#include <cui/dialogs/DialogQuickBetsSetAdd.h>
#include <cui/dialogs/DialogTextInput.h>
#include <controller/AnimationSpeed.h>
#include <controller/AutoOdds.h>
#include <controller/BetSpec.h>
#include <controller/BuddyFollow.h>
#include <controller/QuickBets.h>
#include <controller/CrapsReaders.h>
#include <controller/RollHistoryFormat.h>
#include <controller/PlayerManager.h>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <gen/MoneyUtils.h>
#include <craps/EnumBetName.h>
#include <cctype>
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
// Start a workflow that is an Operation. The previous Operation, if any,
// finished some time ago and was only kept alive until the hook that
// finished it returned. It is safe to drop it now.
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


//----------------------------------------------------------------
//----------------------------------------------------------------

//----------------------------------------------------------------

//----------------------------------------------------------------

//----------------------------------------------------------------

//----------------------------------------------------------------

//----------------------------------------------------------------

//----------------------------------------------------------------

//----------------------------------------------------------------

//----------------------------------------------------------------

//----------------------------------------------------------------

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

//----------------------------------------------------------------

//----------------------------------------------------------------
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
OperationManager::onErrorAcknowledged()
{
    if (current_ && stage_ == Stage::AwaitingErrorAcknowledgement)
    {
        current_->onErrorAcknowledged();
        return;
    }

    if (stage_ != Stage::AwaitingErrorAcknowledgement) return;

    if (type_ == OperationType::Player || type_ == OperationType::Table)
    {
        // Back to the Player or Table menu, still on the stack.
        finish();
        return;
    }

    // DialogError has already popped itself. Now unwind the operation's
    // menus/dialogs to the root screen and release the operation state.
    SurfaceManager::instance().popToRoot();
    finish();
}

//----------------------------------------------------------------
//
// The workflows that are their own Operation classes (see
// cui/operations/). The ones not yet moved are further down.
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

//----------------------------------------------------------------

bool
OperationManager::removeBets()
{
    return begin(std::make_unique<RemoveBetOperation>());
}

//----------------------------------------------------------------

bool
OperationManager::undoLastBet()
{
    return begin(std::make_unique<UndoBetOperation>());
}

//----------------------------------------------------------------

bool
OperationManager::betFlags(Ctrl::BetFlags::Scope scope)
{
    return begin(std::make_unique<BetFlagsOperation>(scope));
}

//----------------------------------------------------------------

bool
OperationManager::applyQuickBets()
{
    return begin(std::make_unique<ApplyQuickBetsOperation>());
}

//----------------------------------------------------------------

bool
OperationManager::quickBetsSetOrAdd()
{
    return begin(std::make_unique<SaveToQuickBetsOperation>());
}

//----------------------------------------------------------------

bool
OperationManager::manageQuickBets()
{
    return begin(std::make_unique<ManageQuickBetsOperation>());
}

//----------------------------------------------------------------

bool
OperationManager::manageAutoOdds()
{
    return begin(std::make_unique<ConfigureAutoOddsOperation>());
}

//----------------------------------------------------------------

bool
OperationManager::autoOddsSetOrUnset()
{
    return begin(std::make_unique<SetAutoOddsOperation>());
}

//----------------------------------------------------------------

void
OperationManager::onQuickBetsChoice(bool accepted, std::size_t index)
{
    if (current_ && stage_ == Stage::AwaitingPick)
    {
        current_->onQuickBetsChoice(accepted, index);
    }
}

//----------------------------------------------------------------
//
// The Controller's answers. One that is not for the current Operation (a
// late answer, or one for another operation) is ignored.
//

void
OperationManager::onRspMakeBet(const Ctrl::RspMakeBet& ev)
{
    route("RspMakeBet", ev, &Operation::onRspMakeBet);
}

void
OperationManager::onRspOddsBet(const Ctrl::RspOddsBet& ev)
{
    route("RspOddsBet", ev, &Operation::onRspOddsBet);
}

void
OperationManager::onRspRemoveBet(const Ctrl::RspRemoveBet& ev)
{
    route("RspRemoveBet", ev, &Operation::onRspRemoveBet);
}

void
OperationManager::onRspUndoBet(const Ctrl::RspUndoBet& ev)
{
    route("RspUndoBet", ev, &Operation::onRspUndoBet);
}

void
OperationManager::onRspSetBetFlags(const Ctrl::RspSetBetFlags& ev)
{
    route("RspSetBetFlags", ev, &Operation::onRspSetBetFlags);
}

void
OperationManager::onRspApplyQuickBets(const Ctrl::RspApplyQuickBets& ev)
{
    route("RspApplyQuickBets", ev, &Operation::onRspApplyQuickBets);
}

void
OperationManager::onRspEditQuickBets(const Ctrl::RspEditQuickBets& ev)
{
    route("RspEditQuickBets", ev, &Operation::onRspEditQuickBets);
}

void
OperationManager::onRspEditAutoOdds(const Ctrl::RspEditAutoOdds& ev)
{
    route("RspEditAutoOdds", ev, &Operation::onRspEditAutoOdds);
}

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

bool
OperationManager::chooseBuddyFollow()
{
    return begin(std::make_unique<BuddyFollowOperation>());
}

//----------------------------------------------------------------

bool
OperationManager::chooseAnimationSpeed()
{
    return begin(std::make_unique<AnimationSpeedOperation>());
}

//----------------------------------------------------------------

bool
OperationManager::chooseRollHistoryFormat()
{
    return begin(std::make_unique<RollHistoryFormatOperation>());
}

//----------------------------------------------------------------

bool
OperationManager::confirmQuit()
{
    return begin(std::make_unique<QuitOperation>());
}

//----------------------------------------------------------------

void
OperationManager::onConfirmationResult(SurfaceBase::OperationResult result)
{
    if (current_ && stage_ == Stage::AwaitingConfirmation)
    {
        current_->onConfirm(result);
        return;
    }

    if (stage_ != Stage::AwaitingConfirmation) return;

    if (type_ == OperationType::Table)
    {
        using R = SurfaceBase::OperationResult;
        if (tableStep_ == TableStep::ConfirmBets)
        {
            if (result == R::Yes) afterBetsConfirmed();
            else                  finish();
        }
        else if (result == R::Yes || result == R::No)   // AskBuddies
        {
            buddiesFollow_ = (result == R::Yes);
            sendTableRequest();
        }
        else
        {
            finish();                 // Esc: don't switch at all
        }
        return;
    }

    if (type_ == OperationType::Player)
    {
        if (result == SurfaceBase::OperationResult::Yes)
        {
            sendPlayerRequest();
        }
        else
        {
            finish();
        }
        return;
    }

}

//----------------------------------------------------------------

void
OperationManager::finish()
{
    // The Operation that is finishing may be the one running right now.
    // Keep it until begin() starts the next one. (A second finish() must
    // not drop it while it is still running.)
    if (current_) retired_ = std::move(current_);

    type_          = OperationType::None;
    stage_         = Stage::Idle;
    correlationId_ = 0;
    targetId_.clear();
    text_.clear();
    description_.clear();
    askingDescription_ = false;
    tableChoices_.clear();
    targetTableId_.clear();
    targetTableName_.clear();
}

//----------------------------------------------------------------

//----------------------------------------------------------------
//
// Player operations, started from MenuPlayers
//
// Each is a short chain of dialogs, then one request to the controller:
//
//   List, ListBuddies   show list
//   Switch              pick, [confirm if bets are on the table], request
//   Rename              type name, request
//   Create              type name, request
//   Delete              pick, confirm, request
//   BuddyJoins/Leaves   pick, request
//
// The dialogs pop themselves before calling back. The Player menu stays
// on the stack, except after a successful Switch, which returns to the
// table.
//
bool
OperationManager::playerOperation(PlayerOp op)
{
    if (isActive()) return false;

    const auto roster = Ctrl::CrapsReaders::readPlayerRoster();
    using Info = Ctrl::PlayerManager::PlayerInfo;

    auto pickable = [&](auto wanted, auto noteFor)
    {
        std::vector<DialogPicker::Item> items;
        for (const Info& p : roster)
        {
            if (wanted(p))
            {
                items.push_back({p.playerId, p.name, noteFor(p),
                                 p.description});
            }
        }
        return items;
    };
    auto userNote  = [](const Info& p) -> std::string
        { return p.isUser ? "(you)" : ""; };
    auto buddyNote = [](const Info& p) -> std::string
        { return p.atTable ? "(at table)" : ""; };
    auto isUserKind  = [](const Info& p)
        { return p.kind == Ctrl::PlayerKind::User; };
    auto isBuddyKind = [](const Info& p)
        { return p.kind == Ctrl::PlayerKind::Buddy; };

    std::vector<DialogPicker::Item> items;
    std::string title;
    auto mode = DialogPicker::Mode::Pick;

    switch (op)
    {
    case PlayerOp::List:
        title = "Your Players";
        mode  = DialogPicker::Mode::Show;
        items = pickable(isUserKind, userNote);
        break;

    case PlayerOp::ListBuddies:
        title = "Buddy Players";
        mode  = DialogPicker::Mode::Show;
        items = pickable(isBuddyKind, buddyNote);
        if (items.empty())
        {
            showNotice("No buddy players", "There are no buddy players.");
            return true;
        }
        break;

    case PlayerOp::Switch:
        title = "Switch to Player";
        items = pickable([&](const Info& p)
                         { return isUserKind(p) && !p.isUser; }, userNote);
        if (items.empty())
        {
            showNotice("No other players",
                       "You have only one player. Use Create New Player "
                       "to make another.");
            return true;
        }
        break;

    case PlayerOp::Delete:
        title = "Delete Player";
        items = pickable([&](const Info& p)
                         { return isUserKind(p) && !p.isUser; }, userNote);
        if (items.empty())
        {
            showNotice("No player to delete",
                       "You can't delete the player you are playing as. "
                       "Switch to another player first.");
            return true;
        }
        break;

    case PlayerOp::BuddyJoins:
        title = "Buddy Joins Table";
        items = pickable([&](const Info& p)
                         { return isBuddyKind(p) && !p.atTable; }, buddyNote);
        if (items.empty())
        {
            showNotice("No buddy to add",
                       "All of the buddy players are already at the table.");
            return true;
        }
        break;

    case PlayerOp::BuddyLeaves:
        title = "Buddy Leaves Table";
        items = pickable([&](const Info& p)
                         { return isBuddyKind(p) && p.atTable; }, buddyNote);
        if (items.empty())
        {
            showNotice("No buddy at the table",
                       "None of the buddy players are at the table.");
            return true;
        }
        break;

    case PlayerOp::Rename:
    case PlayerOp::Create:
        break;   // Asks for a name below
    }

    type_     = OperationType::Player;
    playerOp_ = op;
    text_.clear();
    description_.clear();
    askingDescription_ = false;
    targetId_.clear();
    targetName_.clear();

    if (op == PlayerOp::Rename)
    {
        for (const Info& p : roster)
        {
            if (p.isUser)
            {
                targetId_   = p.playerId;
                targetName_ = p.name;
            }
        }
        askForName("Rename Player", targetName_, targetId_);
        return true;
    }

    if (op == PlayerOp::Create)
    {
        askForName("Create New Player", "", Craps::PlayerId());
        return true;
    }

    stage_ = Stage::AwaitingPick;
    DialogPicker::instance().configure(title, std::move(items), mode);
    SurfaceManager::instance().pushSurface(&DialogPicker::instance());
    return true;
}

//----------------------------------------------------------------
//
// Table operations, started from MenuTable
//
//   List     show the tables, the one you are at marked "(current)"
//   Switch   pick a table, [confirm if bets are on the table],
//            [ask if buddies go along, per the Configure setting], request
//
// After a successful Switch the screen returns to the table.
//
bool
OperationManager::tableOperation(TableOp op)
{
    if (isActive()) return false;

    Craps::TableId currentId;
    Gen::ErrorPass ep;
    (void) Ctrl::CrapsReaders::getActiveCrapsTable(currentId, ep);

    std::vector<DialogPicker::Item> items;
    std::vector<Craps::TableId> choices;
    for (const auto& t : Ctrl::CrapsReaders::readTableList())
    {
        const bool isCurrent = (t.tableId == currentId);
        if (op == TableOp::Switch && isCurrent) continue;

        DialogPicker::Item item;
        item.text        = t.tableName;
        item.note        = (op == TableOp::List && isCurrent) ? "(current)" : "";
        item.description = t.shortDescription;
        item.tag         = choices.size();
        items.push_back(std::move(item));
        choices.push_back(t.tableId);
    }

    if (op == TableOp::Switch && items.empty())
    {
        showNotice("No other tables", "There is no other table to go to.");
        return true;
    }

    type_      = OperationType::Table;
    tableOp_   = op;
    tableStep_ = TableStep::PickTable;
    tableChoices_ = std::move(choices);
    targetTableId_.clear();
    targetTableName_.clear();
    buddiesFollow_ = true;

    stage_ = Stage::AwaitingPick;
    DialogPicker::instance().configure(
        op == TableOp::List ? "Tables" : "Switch to Table", std::move(items),
        op == TableOp::List ? DialogPicker::Mode::Show
                            : DialogPicker::Mode::Pick);
    SurfaceManager::instance().pushSurface(&DialogPicker::instance());
    return true;
}

//----------------------------------------------------------------

void
OperationManager::onTablePick(bool accepted, std::size_t tag)
{
    if (!accepted || tableOp_ != TableOp::Switch ||
        tag >= tableChoices_.size())
    {
        finish();        // Cancelled, or just looking at the list
        return;
    }

    targetTableId_ = tableChoices_[tag];
    for (const auto& t : Ctrl::CrapsReaders::readTableList())
    {
        if (t.tableId == targetTableId_) targetTableName_ = t.tableName;
    }

    unsigned numBets = 0;
    Gen::ErrorPass ep;
    (void) Ctrl::CrapsReaders::readPlayerNumBetsOnTable(
               Ctrl::CrapsReaders::getUserPlayerId(), numBets, ep);
    if (numBets > 0)
    {
        tableStep_ = TableStep::ConfirmBets;
        askConfirmation("Switch to " + targetTableName_ +
                        "? Bets on the table are returned");
    }
    else
    {
        afterBetsConfirmed();
    }
}

//----------------------------------------------------------------
//
// Settle whether the buddies go along, asking if the person said to.
// Nothing to ask when no buddy is at the table.
//
void
OperationManager::afterBetsConfirmed()
{
    unsigned numBuddies = 0;
    for (const auto& p : Ctrl::CrapsReaders::readPlayerRoster())
    {
        if (p.kind == Ctrl::PlayerKind::Buddy && p.atTable) ++numBuddies;
    }

    buddiesFollow_ = true;
    if (numBuddies == 0)
    {
        sendTableRequest();
        return;
    }

    switch (Ctrl::CrapsReaders::readBuddyFollow())
    {
    case Ctrl::BuddyFollow::Follow:
        buddiesFollow_ = true;
        sendTableRequest();
        break;
    case Ctrl::BuddyFollow::Stay:
        buddiesFollow_ = false;
        sendTableRequest();
        break;
    case Ctrl::BuddyFollow::Ask:
        tableStep_ = TableStep::AskBuddies;
        askConfirmation(std::string("Do your buddy ") +
                        (numBuddies == 1 ? "player goes" : "players go") +
                        " to " + targetTableName_ + " with you?");
        break;
    }
}

//----------------------------------------------------------------

void
OperationManager::sendTableRequest()
{
    stage_ = Stage::AwaitingResponse;
    correlationId_ = CuiSender::instance().reqSwitchTable(
        targetTableId_, buddiesFollow_);
}

//----------------------------------------------------------------

void
OperationManager::onRspSwitchTable(const Ctrl::RspSwitchTable& ev)
{
    if (stage_ != Stage::AwaitingResponse ||
        type_  != OperationType::Table    ||
        ev.correlationId != correlationId_)
    {
        LOG_WARN("OperationManager::onRspSwitchTable(): ignoring "
                 "unmatched RspSwitchTable");
        return;
    }

    if (!ev.switched)
    {
        showError(ev.error, "Table Error", "Unable to Switch Table");
        return;
    }

    // Back to the table, now at the new place.
    SurfaceManager::instance().popToRoot();
    finish();

    auto count = [](unsigned n, const char* one, const char* many)
    {
        return std::to_string(n) + " " + (n == 1 ? one : many);
    };
    std::string detail;
    if (ev.numBuddiesFollowed > 0)
    {
        detail += count(ev.numBuddiesFollowed, "buddy player", "buddy players")
                  + " came with you. ";
    }
    if (ev.numBuddiesLeft > 0)
    {
        detail += count(ev.numBuddiesLeft, "buddy player", "buddy players")
                  + " left the game. Use Buddy Joins Table to seat them. ";
    }
    if (ev.numBetsReturned > 0)
    {
        detail += count(ev.numBetsReturned, "bet was", "bets were")
                  + " returned.";
    }
    showNotice("Now at " + ev.toTableName, detail);
}

//----------------------------------------------------------------

//----------------------------------------------------------------
//
// Ask for a player name. The dialog refuses names that are not valid or
// that another player (other than exceptId) already has, and says why,
// so the person can fix it without leaving the dialog.
//
void
OperationManager::askForName(const std::string& title,
                             const std::string& initial,
                             const Craps::PlayerId& exceptId)
{
    const auto roster = Ctrl::CrapsReaders::readPlayerRoster();

    auto validator = [roster, exceptId](const std::string& raw) -> std::string
    {
        const std::string name = trimmed(raw);
        std::string why;
        if (!Ctrl::PlayerManager::checkName(name, why))
        {
            return why;
        }
        for (const auto& p : roster)
        {
            if (p.playerId != exceptId && sameIgnoringCase(p.name, name))
            {
                return "A player named " + p.name + " already exists.";
            }
        }
        return std::string();
    };

    stage_ = Stage::AwaitingText;
    DialogTextInput::instance().configure(
        title, "Name: ", initial,
        Ctrl::PlayerManager::MaxNameLength, std::move(validator));
    SurfaceManager::instance().pushSurface(&DialogTextInput::instance());
}

//----------------------------------------------------------------

void
OperationManager::onPickResult(bool accepted, const Craps::PlayerId& playerId,
                               Craps::BetId betId, std::size_t tag)
{
    if (current_ && stage_ == Stage::AwaitingPick)
    {
        current_->onPick(accepted, playerId, betId, tag);
        return;
    }

    if (stage_ == Stage::AwaitingPick && type_ == OperationType::Table)
    {
        (void)playerId; (void)betId;
        onTablePick(accepted, tag);
        return;
    }

    if (stage_ != Stage::AwaitingPick || type_ != OperationType::Player)
        return;

    if (!accepted)       // Cancelled, or just looking at a list
    {
        finish();
        return;
    }

    targetId_ = playerId;
    targetName_.clear();
    for (const auto& p : Ctrl::CrapsReaders::readPlayerRoster())
    {
        if (p.playerId == playerId) targetName_ = p.name;
    }

    switch (playerOp_)
    {
    case PlayerOp::Switch:
    {
        unsigned numBets = 0;
        Gen::ErrorPass ep;
        (void) Ctrl::CrapsReaders::readPlayerNumBetsOnTable(
                   Ctrl::CrapsReaders::getUserPlayerId(), numBets, ep);
        if (numBets > 0)
        {
            askConfirmation("Switch to " + targetName_ + "? Your " +
                            std::to_string(numBets) +
                            (numBets == 1 ? " bet is" : " bets are") +
                            " returned");
        }
        else
        {
            sendPlayerRequest();
        }
        break;
    }
    case PlayerOp::Delete:
        askConfirmation("Delete player " + targetName_ + "?");
        break;

    case PlayerOp::BuddyJoins:
    case PlayerOp::BuddyLeaves:
        sendPlayerRequest();
        break;

    default:
        finish();     // Not expected: other operations don't pick
        break;
    }
}

//----------------------------------------------------------------

void
OperationManager::onTextResult(bool accepted, const std::string& text)
{
    if (current_ && stage_ == Stage::AwaitingText)
    {
        current_->onText(accepted, text);
        return;
    }

    if (stage_ != Stage::AwaitingText || type_ != OperationType::Player)
        return;

    if (!accepted)
    {
        finish();
        return;
    }
    if (playerOp_ == PlayerOp::Create && !askingDescription_)
    {
        text_ = trimmed(text);
        askForDescription();     // Second question; the first is answered
        return;
    }
    if (playerOp_ == PlayerOp::Create)
    {
        description_ = trimmed(text);
    }
    else
    {
        text_ = trimmed(text);
    }
    sendPlayerRequest();
}

//----------------------------------------------------------------
//
// Creating a player: after the name, an optional short description.
// Esc here cancels the whole creation.
//
void
OperationManager::askForDescription()
{
    askingDescription_ = true;

    auto validator = [](const std::string& raw) -> std::string
    {
        std::string why;
        if (!Ctrl::PlayerManager::checkDescription(trimmed(raw), why))
        {
            return why;
        }
        return std::string();       // Empty is fine
    };

    stage_ = Stage::AwaitingText;
    DialogTextInput::instance().configure(
        "Describe " + text_ + " (optional)", "About: ", "",
        Ctrl::PlayerManager::MaxDescriptionLength, std::move(validator));
    SurfaceManager::instance().pushSurface(&DialogTextInput::instance());
}

//----------------------------------------------------------------

void
OperationManager::askConfirmation(const std::string& question)
{
    stage_ = Stage::AwaitingConfirmation;
    DialogConfirm::instance().configure(question);
    SurfaceManager::instance().pushSurface(&DialogConfirm::instance());
}

//----------------------------------------------------------------

void
OperationManager::sendPlayerRequest()
{
    stage_ = Stage::AwaitingResponse;
    auto& sender = CuiSender::instance();

    switch (playerOp_)
    {
    case PlayerOp::Switch:
        correlationId_ = sender.reqSwitchPlayer(targetId_);
        break;
    case PlayerOp::Rename:
        correlationId_ = sender.reqManagePlayer(
            Ctrl::PlayerAction::Rename, targetId_, text_);
        break;
    case PlayerOp::Create:
        correlationId_ = sender.reqManagePlayer(
            Ctrl::PlayerAction::Create, Craps::PlayerId(), text_, description_);
        break;
    case PlayerOp::Delete:
        correlationId_ = sender.reqManagePlayer(
            Ctrl::PlayerAction::Delete, targetId_, std::string());
        break;
    case PlayerOp::BuddyJoins:
        correlationId_ = sender.reqManagePlayer(
            Ctrl::PlayerAction::BuddyJoins, targetId_, std::string());
        break;
    case PlayerOp::BuddyLeaves:
        correlationId_ = sender.reqManagePlayer(
            Ctrl::PlayerAction::BuddyLeaves, targetId_, std::string());
        break;
    default:
        LOG_ERROR("OperationManager::sendPlayerRequest(): "
                  "operation sends no request");
        finish();
        break;
    }
}

//----------------------------------------------------------------

const char*
OperationManager::playerOpVerb() const
{
    switch (playerOp_)
    {
    case PlayerOp::Switch:      return "Unable to Switch Player";
    case PlayerOp::Rename:      return "Unable to Rename Player";
    case PlayerOp::Create:      return "Unable to Create Player";
    case PlayerOp::Delete:      return "Unable to Delete Player";
    case PlayerOp::BuddyJoins:  return "Unable to Seat Buddy";
    case PlayerOp::BuddyLeaves: return "Unable to Remove Buddy";
    default:                    return "Unable to Complete Operation";
    }
}

//----------------------------------------------------------------

void
OperationManager::onRspSwitchPlayer(const Ctrl::RspSwitchPlayer& ev)
{
    if (stage_ != Stage::AwaitingResponse ||
        type_  != OperationType::Player   ||
        playerOp_ != PlayerOp::Switch     ||
        ev.correlationId != correlationId_)
    {
        LOG_WARN("OperationManager::onRspSwitchPlayer(): ignoring "
                 "unmatched RspSwitchPlayer");
        return;
    }

    if (ev.switched)
    {
        // Back to the table, now played by the new user player.
        SurfaceManager::instance().popToRoot();
        finish();
        return;
    }
    showError(ev.error, "Player Error", playerOpVerb());
}

//----------------------------------------------------------------

void
OperationManager::onRspManagePlayer(const Ctrl::RspManagePlayer& ev)
{
    if (stage_ != Stage::AwaitingResponse ||
        type_  != OperationType::Player   ||
        ev.correlationId != correlationId_)
    {
        LOG_WARN("OperationManager::onRspManagePlayer(): ignoring "
                 "unmatched RspManagePlayer");
        return;
    }

    if (!ev.ok)
    {
        showError(ev.error, "Player Error", playerOpVerb());
        return;
    }

    const PlayerOp op = playerOp_;
    finish();

    if (op == PlayerOp::Create)
    {
        showNotice("Created player " + ev.name,
                   "Use Switch to Player to play as " + ev.name + ".");
    }
    else if (op == PlayerOp::Delete)
    {
        showNotice("Deleted player " + targetName_,
                   "The player file was moved to the trash folder inside "
                   "the players folder.");
    }
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
