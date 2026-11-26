//----------------------------------------------------------------
//
// File: TableOperations.cpp
//
//----------------------------------------------------------------

#include <cui/operations/TableOperations.h>
#include <cui/core/CuiSender.h>
#include <cui/managers/SurfaceManager.h>
#include <controller/BuddyFollow.h>
#include <controller/CrapsReaders.h>

using namespace Cui;

namespace {

// The list of tables for the picker. A table's tag is its place in
// choices. The current table is left out when the person is to pick
// another, and marked when they are only looking.
std::vector<DialogPicker::Item>
tableItems(bool listing, std::vector<Craps::TableId>& choices)
{
    Craps::TableId currentId;
    Gen::ErrorPass ep;
    (void) Ctrl::CrapsReaders::getActiveCrapsTable(currentId, ep);

    std::vector<DialogPicker::Item> items;
    for (const auto& t : Ctrl::CrapsReaders::readTableList())
    {
        const bool isCurrent = (t.tableId == currentId);
        if (!listing && isCurrent) continue;

        DialogPicker::Item item;
        item.text        = t.tableName;
        item.note        = (listing && isCurrent) ? "(current)" : "";
        item.description = t.shortDescription;
        item.tag         = choices.size();
        items.push_back(std::move(item));
        choices.push_back(t.tableId);
    }
    return items;
}

} // namespace

//----------------------------------------------------------------
//
// List Tables
//

void
ListTablesOperation::start()
{
    std::vector<Craps::TableId> choices;
    pickFrom("Tables", tableItems(true, choices), DialogPicker::Mode::Show);
}

//----------------------------------------------------------------

void
ListTablesOperation::onPick(bool, const Craps::PlayerId&, Craps::BetId,
                            std::size_t)
{
    finish();       // Only looking
}

//----------------------------------------------------------------
//
// Switch Table
//

void
SwitchTableOperation::start()
{
    auto items = tableItems(false, choices_);
    if (items.empty())
    {
        showNotice("No other tables", "There is no other table to go to.");
        finish();
        return;
    }

    step_ = Step::PickTable;
    pickFrom("Switch to Table", std::move(items));
}

//----------------------------------------------------------------

void
SwitchTableOperation::onPick(bool accepted, const Craps::PlayerId&,
                             Craps::BetId, std::size_t tag)
{
    if (!accepted || tag >= choices_.size())
    {
        finish();        // Cancelled
        return;
    }

    targetId_ = choices_[tag];
    for (const auto& t : Ctrl::CrapsReaders::readTableList())
    {
        if (t.tableId == targetId_) targetName_ = t.tableName;
    }

    // Leaving the table puts the bets back. Say so first when there are some.
    unsigned numBets = 0;
    Gen::ErrorPass ep;
    (void) Ctrl::CrapsReaders::readPlayerNumBetsOnTable(
               Ctrl::CrapsReaders::getUserPlayerId(), numBets, ep);
    if (numBets > 0)
    {
        step_ = Step::ConfirmBets;
        askConfirmation("Switch to " + targetName_ +
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
SwitchTableOperation::afterBetsConfirmed()
{
    unsigned numBuddies = 0;
    for (const auto& p : Ctrl::CrapsReaders::readPlayerRoster())
    {
        if (p.kind == Ctrl::PlayerKind::Buddy && p.atTable) ++numBuddies;
    }

    buddiesFollow_ = true;
    if (numBuddies == 0)
    {
        sendRequest();
        return;
    }

    switch (Ctrl::CrapsReaders::readBuddyFollow())
    {
    case Ctrl::BuddyFollow::Follow:
        buddiesFollow_ = true;
        sendRequest();
        break;
    case Ctrl::BuddyFollow::Stay:
        buddiesFollow_ = false;
        sendRequest();
        break;
    case Ctrl::BuddyFollow::Ask:
        askBuddies(numBuddies);
        break;
    }
}

//----------------------------------------------------------------

void
SwitchTableOperation::askBuddies(unsigned numBuddies)
{
    step_ = Step::AskBuddies;
    askConfirmation(std::string("Do your buddy ") +
                    (numBuddies == 1 ? "player goes" : "players go") +
                    " to " + targetName_ + " with you?");
}

//----------------------------------------------------------------

void
SwitchTableOperation::onConfirm(SurfaceBase::OperationResult result)
{
    using R = SurfaceBase::OperationResult;

    if (step_ == Step::ConfirmBets)
    {
        if (result == R::Yes) afterBetsConfirmed();
        else                  finish();
        return;
    }

    // AskBuddies: Yes and No both switch. Esc is don't switch at all.
    if (result == R::Yes || result == R::No)
    {
        buddiesFollow_ = (result == R::Yes);
        sendRequest();
    }
    else
    {
        finish();
    }
}

//----------------------------------------------------------------

void
SwitchTableOperation::sendRequest()
{
    awaitResponse(CuiSender::instance().reqSwitchTable(
        targetId_, buddiesFollow_));
}

//----------------------------------------------------------------

void
SwitchTableOperation::onRspSwitchTable(const Ctrl::RspSwitchTable& rsp)
{
    if (!rsp.switched)
    {
        showError(rsp.error, "Table Error", "Unable to Switch Table");
        return;
    }

    // Back to the table, now at the new place.
    finishAtRoot();
    showNotice("Now at " + rsp.toTableName, describeSwitch(rsp));
}

//----------------------------------------------------------------
//
// What happened to the buddies and the bets, in words
//
std::string
SwitchTableOperation::describeSwitch(const Ctrl::RspSwitchTable& rsp)
{
    auto count = [](unsigned n, const char* one, const char* many)
    {
        return std::to_string(n) + " " + (n == 1 ? one : many);
    };

    std::string detail;
    if (rsp.numBuddiesFollowed > 0)
    {
        detail += count(rsp.numBuddiesFollowed,
                        "buddy player", "buddy players") + " came with you. ";
    }
    if (rsp.numBuddiesLeft > 0)
    {
        detail += count(rsp.numBuddiesLeft,
                        "buddy player", "buddy players") +
                  " left the game. Use Buddy Joins Table to seat them. ";
    }
    if (rsp.numBetsReturned > 0)
    {
        detail += count(rsp.numBetsReturned, "bet was", "bets were") +
                  " returned.";
    }
    return detail;
}

//----------------------------------------------------------------

void
SwitchTableOperation::onErrorAcknowledged()
{
    finish();       // Back to the Tables menu, still on the stack
}

//----------------------------------------------------------------
