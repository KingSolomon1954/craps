//----------------------------------------------------------------
//
// File: OperationManager.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/AutoOdds.h>
#include <controller/BetFlags.h>
#include <controller/BetSpec.h>
#include <controller/GameEvents.h>
#include <cui/bases/SurfaceBase.h>
#include <cui/managers/CarrierBet.h>
#include <cui/operations/Operation.h>
#include <gen/ErrorPass.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Cui {

/*-----------------------------------------------------------*//**

@class OperationManager

@brief Runs the workflows the person starts from the menus, one at a time.

A workflow is a short chain of dialogs, usually ending with a request to
the Controller and its answer. Each workflow is an Operation (see
cui/operations/), made when it starts and discarded when it finishes. This
class:

- starts the workflow the menu asked for, if none is running;
- remembers which request the answer is awaited for (the correlation id),
  and what the workflow is waiting on (the stage);
- passes each dialog answer and each Controller response to the running
  Operation, and ignores a response that is not the one awaited;
- keeps the AutoOdds status line.

It is the one place the menus, dialogs and WorkOrderDispatcher call. What
a workflow does is in its Operation.

Everything runs on the CUI thread.

*/
class OperationManager
{
public:
    /// @name Lifecycle
    /// @{
    static OperationManager& instance();

    OperationManager(const OperationManager&) = delete;
    OperationManager& operator=(const OperationManager&) = delete;
    OperationManager(OperationManager&&) = delete;
    OperationManager& operator=(OperationManager&&) = delete;
    /// @}

    /// The player operations on MenuPlayers
    enum class PlayerOp
    {
        List,          ///< Show the user players
        Switch,        ///< Become another user player
        Rename,        ///< Rename the player you are playing as
        Create,        ///< Make a new user player
        Delete,        ///< Delete a user player
        ListBuddies,   ///< Show the buddy players
        BuddyJoins,    ///< A buddy sits at the table
        BuddyLeaves    ///< A buddy leaves the table
    };

    /// The table operations on MenuTable
    enum class TableOp
    {
        List,          ///< Show the tables
        Switch         ///< Move to another table
    };

    /// @name Workflows
    /// Each starts a workflow. All but makeBet() answer false, and do
    /// nothing, if another workflow is running.
    /// @{
    bool tableOperation(TableOp op);    // Table menu operations
    bool playerOperation(PlayerOp op);  // Player menu operations
    void makeBet();                     // Make a bet, or put odds on one
    bool removeBets();                  // Pick one of your bets to take down
    bool undoLastBet();                 // Take back your latest bet change
    bool betFlags(Ctrl::BetFlags::Scope scope); // Turn bets off or on
    bool applyQuickBets();              // Pick a QuickBets entry and apply it
    bool quickBetsSetOrAdd();           // Save the bet being entered
    bool manageQuickBets();             // Rename, change, trim, delete entries
    bool manageAutoOdds();              // Turn on or off, set, clear AutoOdds
    bool autoOddsSetOrUnset();          // AutoOdds for the odds being entered
    bool chooseBuddyFollow();           // What buddies do on a table switch
    bool chooseAnimationSpeed();        // The dice animation speed
    bool chooseRollHistoryFormat();     // How rolls are listed
    bool confirmQuit();                 // Confirmation to quit
    /// @}

    /// @name Events
    /// Called only on the CUI thread, by WorkOrderDispatcher and the
    /// dialogs.
    /// @{
    void onRspMakeBet(const Ctrl::RspMakeBet& response);
    void onRspOddsBet(const Ctrl::RspOddsBet& response);
    void onRspRemoveBet(const Ctrl::RspRemoveBet& response);
    void onRspUndoBet(const Ctrl::RspUndoBet& response);
    void onRspSetBetFlags(const Ctrl::RspSetBetFlags& response);
    void onRspApplyQuickBets(const Ctrl::RspApplyQuickBets& response);
    void onRspEditQuickBets(const Ctrl::RspEditQuickBets& response);
    void onRspEditAutoOdds(const Ctrl::RspEditAutoOdds& response);
    void onRspSwitchPlayer(const Ctrl::RspSwitchPlayer& response);
    void onRspManagePlayer(const Ctrl::RspManagePlayer& response);
    void onRspSwitchTable(const Ctrl::RspSwitchTable& response);
    void onAutoOddsApplied(const Ctrl::UslAutoOddsApplied& response);
    void clearAutoOddsStatus();
    void onQuickBetsChoice(bool accepted, std::size_t index);
    void onConfirmationResult(SurfaceBase::OperationResult result);
    void onPickResult(bool accepted, const Craps::PlayerId& playerId,
                      Craps::BetId betId = 0, std::size_t tag = 0);
    void onTextResult(bool accepted, const std::string& text);
    void onErrorAcknowledged();
    /// @}

    /// @name Observers
    /// @{
    bool isActive() const;

    /// What AutoOdds just did, in a few words for the status line; empty
    /// if nothing yet this roll.
    const std::string& autoOddsStatus() const;
    /// @}

private:
    friend class Operation;   // Its protected helpers use the manager's state

    /// What the running workflow is waiting for
    enum class Stage
    {
        Idle,                          ///< No workflow is running
        AwaitingResponse,              ///< The Controller's answer
        AwaitingErrorAcknowledgement,  ///< The error dialog to be closed
        AwaitingConfirmation,          ///< A yes or no
        AwaitingPick,                  ///< A pick list or similar dialog
        AwaitingText                   ///< A text prompt
    };

    OperationManager() = default;

    bool begin(std::unique_ptr<Operation> op);
    void finish();

    template <class Rsp>
    void route(const char* name, const Rsp& ev,
               void (Operation::*hook)(const Rsp&));

    void showError(const Gen::ErrorPass& error,
                   const char* defaultType,
                   const char* defaultName);
    void showNotice(const std::string& brief, const std::string& detail);

    std::unique_ptr<Operation> current_;   ///< The running workflow, if any
    std::unique_ptr<Operation> retired_;   ///< Finished; freed by the next begin()

    Stage               stage_         = Stage::Idle;
    Ctrl::CorrelationId correlationId_ = 0;   ///< Of the request awaited

    std::string autoOddsStatus_;   ///< What AutoOdds just did, for the status line
};

} // namespace Cui

//----------------------------------------------------------------
