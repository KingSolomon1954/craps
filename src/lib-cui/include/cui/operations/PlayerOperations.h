//----------------------------------------------------------------
//
// File: PlayerOperations.h
//
// The operations of the Players menu (MenuPlayers). Each is a short
// chain of dialogs, then one request to the Controller:
//
//   Your Players, Buddy Players   show a list
//   Switch                        pick, [confirm if bets are on the
//                                 table], request
//   Rename                        type name, request
//   Create                        type name and description, request
//   Delete                        pick, confirm, request
//   Buddy Joins, Buddy Leaves     pick, request
//
// The dialogs pop themselves before calling back. The Players menu stays
// on the stack, except after a successful Switch, which returns to the
// table.
//
//----------------------------------------------------------------

#pragma once

#include <cui/operations/Operation.h>
#include <controller/PlayerManager.h>

namespace Cui {

/// Show the user's players, or the buddy players. Nothing is sent.
class ShowPlayersOperation : public Operation
{
public:
    explicit ShowPlayersOperation(bool buddies) : buddies_(buddies) {}

    void start() override;
    void onPick(bool accepted, const Craps::PlayerId& playerId,
                Craps::BetId betId, std::size_t tag) override;

private:
    const bool buddies_;
};

/// What the operations that send a ReqManagePlayer share: the request, the
/// answer, and where an error leaves the person (on the Players menu).
class PlayerOperation : public Operation
{
public:
    void onRspManagePlayer(const Ctrl::RspManagePlayer& rsp) override;
    void onErrorAcknowledged() override;

protected:
    using Info = Ctrl::PlayerManager::PlayerInfo;

    void sendManage(Ctrl::PlayerAction action, const Craps::PlayerId& id,
                    const std::string& name,
                    const std::string& description = std::string());

    /// Ask for a player name. The dialog refuses names that are not valid
    /// or that another player (other than exceptId) already has, and says
    /// why, so the person can fix it without leaving the dialog.
    void askName(const std::string& title, const std::string& initial,
                 const Craps::PlayerId& exceptId);

    /// The title of the error dialog if the request fails
    virtual const char* failureName() const = 0;

    /// The request succeeded. The operation has already finished.
    virtual void onManaged(const Ctrl::RspManagePlayer& rsp);
};

/// An operation that begins by picking one player from the roster. The
/// subclass says which players are offered and what is done with the one
/// picked.
class PickPlayerOperation : public PlayerOperation
{
public:
    void start() override;
    void onPick(bool accepted, const Craps::PlayerId& playerId,
                Craps::BetId betId, std::size_t tag) override;

protected:
    virtual const char* title() const = 0;
    virtual bool wants(const Info& player) const = 0;
    virtual std::string noteFor(const Info& player) const = 0;

    /// Nobody can be picked. Say why.
    virtual void showNothingToPick() = 0;

    /// A player was picked: targetId_ and targetName_ are set.
    virtual void onPicked() = 0;

    Craps::PlayerId targetId_;
    std::string     targetName_;
};

/// Switch to another of the user's players.
class SwitchPlayerOperation : public PickPlayerOperation
{
public:
    void onConfirm(SurfaceBase::OperationResult result) override;
    void onRspSwitchPlayer(const Ctrl::RspSwitchPlayer& rsp) override;

private:
    const char* title() const override { return "Switch to Player"; }
    bool wants(const Info& p) const override;
    std::string noteFor(const Info& p) const override;
    const char* failureName() const override { return "Unable to Switch Player"; }
    void showNothingToPick() override;
    void onPicked() override;

    void sendSwitch();
};

/// Delete one of the user's other players, after a confirmation.
class DeletePlayerOperation : public PickPlayerOperation
{
public:
    void onConfirm(SurfaceBase::OperationResult result) override;

private:
    const char* title() const override { return "Delete Player"; }
    bool wants(const Info& p) const override;
    std::string noteFor(const Info& p) const override;
    const char* failureName() const override { return "Unable to Delete Player"; }
    void showNothingToPick() override;
    void onPicked() override;
    void onManaged(const Ctrl::RspManagePlayer& rsp) override;
};

/// Seat a buddy player at the table.
class BuddyJoinsOperation : public PickPlayerOperation
{
private:
    const char* title() const override { return "Buddy Joins Table"; }
    bool wants(const Info& p) const override;
    std::string noteFor(const Info& p) const override;
    const char* failureName() const override { return "Unable to Seat Buddy"; }
    void showNothingToPick() override;
    void onPicked() override;
};

/// Take a buddy player from the table.
class BuddyLeavesOperation : public PickPlayerOperation
{
private:
    const char* title() const override { return "Buddy Leaves Table"; }
    bool wants(const Info& p) const override;
    std::string noteFor(const Info& p) const override;
    const char* failureName() const override { return "Unable to Remove Buddy"; }
    void showNothingToPick() override;
    void onPicked() override;
};

/// Rename the user player.
class RenamePlayerOperation : public PlayerOperation
{
public:
    void start() override;
    void onText(bool accepted, const std::string& text) override;

private:
    const char* failureName() const override { return "Unable to Rename Player"; }

    Craps::PlayerId playerId_;      // The user player
};

/// Create a user player: a name, then an optional short description.
/// Esc at either question cancels the whole creation.
class CreatePlayerOperation : public PlayerOperation
{
public:
    void start() override;
    void onText(bool accepted, const std::string& text) override;

private:
    const char* failureName() const override { return "Unable to Create Player"; }
    void onManaged(const Ctrl::RspManagePlayer& rsp) override;
    void askDescription();

    std::string name_;
    bool        askingDescription_ = false;
};

} // namespace Cui

//----------------------------------------------------------------
