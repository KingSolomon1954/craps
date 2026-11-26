//----------------------------------------------------------------
//
// File: PlayerOperations.cpp
//
//----------------------------------------------------------------

#include <cui/operations/PlayerOperations.h>
#include <cui/core/CuiSender.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/operations/OperationText.h>
#include <controller/CrapsReaders.h>

using namespace Cui;

namespace {

using Info = Ctrl::PlayerManager::PlayerInfo;

bool isUserKind(const Info& p)  { return p.kind == Ctrl::PlayerKind::User; }
bool isBuddyKind(const Info& p) { return p.kind == Ctrl::PlayerKind::Buddy; }

std::string userNote(const Info& p)  { return p.isUser ? "(you)" : ""; }
std::string buddyNote(const Info& p) { return p.atTable ? "(at table)" : ""; }

// Players that are not the one being played as
bool otherUserPlayer(const Info& p) { return isUserKind(p) && !p.isUser; }

} // namespace

//----------------------------------------------------------------
//
// Show Players
//

void
ShowPlayersOperation::start()
{
    std::vector<DialogPicker::Item> items;
    for (const Info& p : Ctrl::CrapsReaders::readPlayerRoster())
    {
        if (buddies_ ? isBuddyKind(p) : isUserKind(p))
        {
            items.push_back({p.playerId, p.name,
                             buddies_ ? buddyNote(p) : userNote(p),
                             p.description});
        }
    }

    if (buddies_ && items.empty())
    {
        showNotice("No buddy players", "There are no buddy players.");
        finish();
        return;
    }
    pickFrom(buddies_ ? "Buddy Players" : "Your Players", std::move(items),
             DialogPicker::Mode::Show);
}

//----------------------------------------------------------------

void
ShowPlayersOperation::onPick(bool, const Craps::PlayerId&, Craps::BetId,
                             std::size_t)
{
    finish();       // Only looking
}

//----------------------------------------------------------------
//
// What the operations that manage players share
//

void
PlayerOperation::sendManage(Ctrl::PlayerAction action,
                            const Craps::PlayerId& id,
                            const std::string& name,
                            const std::string& description)
{
    awaitResponse(CuiSender::instance().reqManagePlayer(
        action, id, name, description));
}

//----------------------------------------------------------------

void
PlayerOperation::onRspManagePlayer(const Ctrl::RspManagePlayer& rsp)
{
    if (!rsp.ok)
    {
        showError(rsp.error, "Player Error", failureName());
        return;
    }

    finish();
    onManaged(rsp);
}

//----------------------------------------------------------------

void
PlayerOperation::onManaged(const Ctrl::RspManagePlayer&)
{
    // Nothing to say: the lists show the change
}

//----------------------------------------------------------------

void
PlayerOperation::onErrorAcknowledged()
{
    finish();       // Back to the Players menu, still on the stack
}

//----------------------------------------------------------------

void
PlayerOperation::askName(const std::string& title, const std::string& initial,
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

    askText(title, "Name: ", initial,
            Ctrl::PlayerManager::MaxNameLength, std::move(validator));
}

//----------------------------------------------------------------
//
// Operations that pick a player
//

void
PickPlayerOperation::start()
{
    std::vector<DialogPicker::Item> items;
    for (const Info& p : Ctrl::CrapsReaders::readPlayerRoster())
    {
        if (wants(p))
        {
            items.push_back({p.playerId, p.name, noteFor(p), p.description});
        }
    }

    if (items.empty())
    {
        showNothingToPick();
        finish();
        return;
    }
    pickFrom(title(), std::move(items));
}

//----------------------------------------------------------------

void
PickPlayerOperation::onPick(bool accepted, const Craps::PlayerId& playerId,
                            Craps::BetId, std::size_t)
{
    if (!accepted)
    {
        finish();       // Cancelled
        return;
    }

    targetId_ = playerId;
    targetName_.clear();
    for (const auto& p : Ctrl::CrapsReaders::readPlayerRoster())
    {
        if (p.playerId == playerId) targetName_ = p.name;
    }
    onPicked();
}

//----------------------------------------------------------------
//
// Switch Player
//

bool
SwitchPlayerOperation::wants(const Info& p) const
{
    return otherUserPlayer(p);
}

std::string
SwitchPlayerOperation::noteFor(const Info& p) const
{
    return userNote(p);
}

//----------------------------------------------------------------

void
SwitchPlayerOperation::showNothingToPick()
{
    showNotice("No other players",
               "You have only one player. Use Create New Player "
               "to make another.");
}

//----------------------------------------------------------------
//
// Switching puts the user's bets back, so say so first when there are
// any.
//
void
SwitchPlayerOperation::onPicked()
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
        sendSwitch();
    }
}

//----------------------------------------------------------------

void
SwitchPlayerOperation::onConfirm(SurfaceBase::OperationResult result)
{
    if (result == SurfaceBase::OperationResult::Yes) sendSwitch();
    else                                             finish();
}

//----------------------------------------------------------------

void
SwitchPlayerOperation::sendSwitch()
{
    awaitResponse(CuiSender::instance().reqSwitchPlayer(targetId_));
}

//----------------------------------------------------------------

void
SwitchPlayerOperation::onRspSwitchPlayer(const Ctrl::RspSwitchPlayer& rsp)
{
    if (rsp.switched)
    {
        // Back to the table, now played by the new user player.
        finishAtRoot();
        return;
    }
    showError(rsp.error, "Player Error", failureName());
}

//----------------------------------------------------------------
//
// Delete Player
//

bool
DeletePlayerOperation::wants(const Info& p) const
{
    return otherUserPlayer(p);
}

std::string
DeletePlayerOperation::noteFor(const Info& p) const
{
    return userNote(p);
}

//----------------------------------------------------------------

void
DeletePlayerOperation::showNothingToPick()
{
    showNotice("No player to delete",
               "You can't delete the player you are playing as. "
               "Switch to another player first.");
}

//----------------------------------------------------------------

void
DeletePlayerOperation::onPicked()
{
    askConfirmation("Delete player " + targetName_ + "?");
}

//----------------------------------------------------------------

void
DeletePlayerOperation::onConfirm(SurfaceBase::OperationResult result)
{
    if (result == SurfaceBase::OperationResult::Yes)
    {
        sendManage(Ctrl::PlayerAction::Delete, targetId_, std::string());
    }
    else
    {
        finish();
    }
}

//----------------------------------------------------------------

void
DeletePlayerOperation::onManaged(const Ctrl::RspManagePlayer&)
{
    showNotice("Deleted player " + targetName_,
               "The player file was moved to the trash folder inside "
               "the players folder.");
}

//----------------------------------------------------------------
//
// Buddy Joins, Buddy Leaves
//

bool
BuddyJoinsOperation::wants(const Info& p) const
{
    return isBuddyKind(p) && !p.atTable;
}

std::string
BuddyJoinsOperation::noteFor(const Info& p) const
{
    return buddyNote(p);
}

void
BuddyJoinsOperation::showNothingToPick()
{
    showNotice("No buddy to add",
               "All of the buddy players are already at the table.");
}

void
BuddyJoinsOperation::onPicked()
{
    sendManage(Ctrl::PlayerAction::BuddyJoins, targetId_, std::string());
}

//----------------------------------------------------------------

bool
BuddyLeavesOperation::wants(const Info& p) const
{
    return isBuddyKind(p) && p.atTable;
}

std::string
BuddyLeavesOperation::noteFor(const Info& p) const
{
    return buddyNote(p);
}

void
BuddyLeavesOperation::showNothingToPick()
{
    showNotice("No buddy at the table",
               "None of the buddy players are at the table.");
}

void
BuddyLeavesOperation::onPicked()
{
    sendManage(Ctrl::PlayerAction::BuddyLeaves, targetId_, std::string());
}

//----------------------------------------------------------------
//
// Rename Player
//

void
RenamePlayerOperation::start()
{
    std::string name;
    for (const Info& p : Ctrl::CrapsReaders::readPlayerRoster())
    {
        if (p.isUser)
        {
            playerId_ = p.playerId;
            name      = p.name;
        }
    }
    askName("Rename Player", name, playerId_);
}

//----------------------------------------------------------------

void
RenamePlayerOperation::onText(bool accepted, const std::string& text)
{
    if (!accepted)
    {
        finish();
        return;
    }
    sendManage(Ctrl::PlayerAction::Rename, playerId_, trimmed(text));
}

//----------------------------------------------------------------
//
// Create Player
//

void
CreatePlayerOperation::start()
{
    askName("Create New Player", std::string(), Craps::PlayerId());
}

//----------------------------------------------------------------

void
CreatePlayerOperation::onText(bool accepted, const std::string& text)
{
    if (!accepted)
    {
        finish();
        return;
    }

    if (!askingDescription_)
    {
        name_ = trimmed(text);
        askDescription();       // Second question; the first is answered
        return;
    }
    sendManage(Ctrl::PlayerAction::Create, Craps::PlayerId(), name_,
               trimmed(text));
}

//----------------------------------------------------------------

void
CreatePlayerOperation::askDescription()
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

    askText("Describe " + name_ + " (optional)", "About: ", "",
            Ctrl::PlayerManager::MaxDescriptionLength, std::move(validator));
}

//----------------------------------------------------------------

void
CreatePlayerOperation::onManaged(const Ctrl::RspManagePlayer& rsp)
{
    showNotice("Created player " + rsp.name,
               "Use Switch to Player to play as " + rsp.name + ".");
}

//----------------------------------------------------------------
