//----------------------------------------------------------------
//
// File: TableOperations.h
//
// The operations of the Tables menu (MenuTable).
//
//----------------------------------------------------------------

#pragma once

#include <cui/operations/Operation.h>
#include <craps/CrapsTypes.h>

namespace Cui {

/// Show the tables, the one you are at marked "(current)". Nothing is sent.
class ListTablesOperation : public Operation
{
public:
    void start() override;
    void onPick(bool accepted, const Craps::PlayerId& playerId,
                Craps::BetId betId, std::size_t tag) override;
};

/// Switch to another table:
///
///   pick a table
///   [confirm, if bets are on the table: they are returned]
///   [ask if the buddies go along, if the Configure setting says to ask
///    and a buddy is at the table]
///   request
///
/// After a successful switch the screen returns to the table. An error
/// leaves the person on the Tables menu. See "Table Switching" in the
/// design document.
class SwitchTableOperation : public Operation
{
public:
    void start() override;
    void onPick(bool accepted, const Craps::PlayerId& playerId,
                Craps::BetId betId, std::size_t tag) override;
    void onConfirm(SurfaceBase::OperationResult result) override;
    void onRspSwitchTable(const Ctrl::RspSwitchTable& rsp) override;
    void onErrorAcknowledged() override;

private:
    enum class Step { PickTable, ConfirmBets, AskBuddies };

    void afterBetsConfirmed();
    void askBuddies(unsigned numBuddies);
    void sendRequest();
    static std::string describeSwitch(const Ctrl::RspSwitchTable& rsp);

    Step                        step_ = Step::PickTable;
    std::vector<Craps::TableId> choices_;      // What the list offered, by tag
    Craps::TableId              targetId_;
    std::string                 targetName_;
    bool                        buddiesFollow_ = true;
};

} // namespace Cui

//----------------------------------------------------------------
