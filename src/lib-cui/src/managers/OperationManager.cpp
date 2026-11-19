//----------------------------------------------------------------
//
// File: OperationManager.cpp
//
//----------------------------------------------------------------

#include <cui/managers/OperationManager.h>
#include <cui/managers/CarrierBet.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/core/CuiSender.h>
#include <cui/dialogs/DialogBetAmount.h>
#include <cui/dialogs/DialogConfirm.h>
#include <cui/dialogs/DialogError.h>
#include <cui/dialogs/DialogMessage.h>
#include <cui/dialogs/DialogPickPlayer.h>
#include <cui/dialogs/DialogTextInput.h>
#include <controller/CrapsReaders.h>
#include <controller/PlayerManager.h>
#include <algorithm>
#include <cctype>
#include <gen/Logger.h>
#include <string>

using namespace Cui;

namespace {

std::string
trimmed(const std::string& s)
{
    std::size_t b = 0, e = s.size();
    while (b < e && std::isspace(static_cast<unsigned char>(s[b]))) b++;
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) e--;
    return s.substr(b, e - b);
}

bool
sameIgnoringCase(const std::string& a, const std::string& b)
{
    return a.size() == b.size() &&
        std::equal(a.begin(), a.end(), b.begin(), [](char x, char y) {
            return std::tolower(static_cast<unsigned char>(x)) ==
                   std::tolower(static_cast<unsigned char>(y)); });
}

} // namespace

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
    LOG_TRACE("OperationManager::doOddsBet(): request correlationId = " +
              std::to_string(correlationId_));
    
    correlationId_ = CuiSender::instance().reqOddsBet(
                         cb.getPlayerId(),
                         cb.getBetId(),
                         cb.getOddsAmount());

}

//----------------------------------------------------------------

void
OperationManager::onRspMakeBet(const Ctrl::RspMakeBet& ev)
{
    LOG_TRACE("OperationManager::onRspMakeBet() "
              "correlationId = " + std::to_string(correlationId_));
             
    if (stage_ != Stage::AwaitingResponse ||
        type_  != OperationType::MakeBet  ||
        ev.correlationId != correlationId_)
    {
        LOG_WARN("OperationManager::onRspMakeBet(): ignoring "
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
OperationManager::onRspOddsBet(const Ctrl::RspOddsBet& ev)
{
    LOG_TRACE("OperationManager::onRspOddsBet() "
              "correlationId = " + std::to_string(correlationId_));
             
    if (stage_ != Stage::AwaitingResponse ||
        type_  != OperationType::MakeBet  ||
        ev.correlationId != correlationId_)
    {
        LOG_WARN("OperationManager::onRspMakeBet(): ignoring "
                 "unmatched RspMakeBet: rsp.correlationId = "       +
                 std::to_string(ev.correlationId)                   +
                 ", have correlationId_ = "                         +
                 std::to_string(correlationId_));
        return;
    }

    if (ev.accepted)
    {
        // ReqOddsBet has succeeded. Unwind the active betting workflow.
        SurfaceManager::instance().popToRoot();
        finish();
        return;
    }

    showError(ev.error);
}

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
    if (stage_ != Stage::AwaitingErrorAcknowledgement) return;

    if (type_ == OperationType::Player)
    {
        // Back to the Player menu, which is still on the stack.
        finish();
        return;
    }

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
    if (stage_ != Stage::AwaitingConfirmation) return;

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

    if (type_ != OperationType::Quit) return;

    if (result == SurfaceBase::OperationResult::Yes)
    {
        CuiSender::instance().reqTerminate();
    }
    finish();
}

//----------------------------------------------------------------
//
// Operation to Set Up an AutoFill
//
// DialogBetAmount calls this. This is a single-step operation, unlike
// other workflows that have a lifetime across several menus.
//
void
OperationManager::autoFill()
{
    const auto& cb = CarrierBet::instance();
    CuiSender::instance().reqAutoFill(
        cb.getBetName(),
        cb.getPivot(),
        cb.isOddsBet(),
        cb.isOddsBet() ? cb.getOddsAmount() : cb.getContractAmount());
}

//----------------------------------------------------------------
//
// Operation to Set Up a QuickBet
//
// DialogBetAmount calls this. This is a single-step operation, unlike
// other workflows that have a lifetime across several menus.
//

void
OperationManager::quickBet()
{
    const auto& cb = CarrierBet::instance();
    CuiSender::instance().reqQuickBet(
        cb.getBetName(),
        cb.getPivot(),
        cb.isOddsBet(),
        cb.isOddsBet() ? cb.getOddsAmount() : cb.getContractAmount());
}

//----------------------------------------------------------------

void
OperationManager::finish()
{
    type_          = OperationType::None;
    stage_         = Stage::Idle;
    correlationId_ = 0;
    targetId_.clear();
    text_.clear();
    description_.clear();
    askingDescription_ = false;
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
        std::vector<DialogPickPlayer::Item> items;
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

    std::vector<DialogPickPlayer::Item> items;
    std::string title;
    auto mode = DialogPickPlayer::Mode::Pick;

    switch (op)
    {
    case PlayerOp::List:
        title = "Your Players";
        mode  = DialogPickPlayer::Mode::Show;
        items = pickable(isUserKind, userNote);
        break;

    case PlayerOp::ListBuddies:
        title = "Buddy Players";
        mode  = DialogPickPlayer::Mode::Show;
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
    DialogPickPlayer::instance().configure(title, std::move(items), mode);
    SurfaceManager::instance().pushSurface(&DialogPickPlayer::instance());
    return true;
}

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
OperationManager::onPickResult(bool accepted, const Craps::PlayerId& playerId)
{
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
