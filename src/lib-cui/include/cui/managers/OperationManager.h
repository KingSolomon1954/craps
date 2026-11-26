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

    /// @name Modifiers
    /// @{
    bool tableOperation(TableOp op);    // Workflow: Table menu operations
    bool chooseBuddyFollow();           // Workflow: Pick what buddies do on a switch
    bool playerOperation(PlayerOp op);  // Workflow: Player menu operations
    void makeBet();      // Workflow: Asynchronous bet operation
    bool removeBets();   // Workflow: Pick one of your bets to take down
    bool undoLastBet();  // Workflow: Take back your latest bet change
    bool betFlags(Ctrl::BetFlags::Scope scope); // Workflow: Turn bets off or on
    bool applyQuickBets(); // Workflow: Pick a QuickBets entry and apply it
    bool quickBetsSetOrAdd(); // Workflow: Save the bet being entered
    bool manageQuickBets();   // Workflow: Rename, change, trim, delete entries
    bool manageAutoOdds();    // Workflow: Turn on or off, set, clear AutoOdds
    bool autoOddsSetOrUnset(); // Workflow: AutoOdds for the odds being entered
    bool chooseAnimationSpeed(); // Workflow: Pick the dice animation speed
    bool chooseRollHistoryFormat(); // Workflow: Pick how rolls are listed
    bool confirmQuit();  // Workflow: Confirmation to quit application

    // Called only on the CUI thread by WorkOrderDispatcher / dialogs.
    void onRspMakeBet(const Ctrl::RspMakeBet& response);
    void onRspOddsBet(const Ctrl::RspOddsBet& response);
    void onRspRemoveBet(const Ctrl::RspRemoveBet& response);
    void onRspUndoBet(const Ctrl::RspUndoBet& response);
    void onRspSetBetFlags(const Ctrl::RspSetBetFlags& response);
    void onRspApplyQuickBets(const Ctrl::RspApplyQuickBets& response);
    void onRspEditQuickBets(const Ctrl::RspEditQuickBets& response);
    void onRspEditAutoOdds(const Ctrl::RspEditAutoOdds& response);
    void onAutoOddsApplied(const Ctrl::UslAutoOddsApplied& response);
    void clearAutoOddsStatus();
    void onQuickBetsChoice(bool accepted, std::size_t index);
    void onConfirmationResult(SurfaceBase::OperationResult result);
    void onPickResult(bool accepted, const Craps::PlayerId& playerId,
                      Craps::BetId betId = 0, std::size_t tag = 0);
    void onTextResult(bool accepted, const std::string& text);
    void onRspSwitchPlayer(const Ctrl::RspSwitchPlayer& response);
    void onRspManagePlayer(const Ctrl::RspManagePlayer& response);
    void onRspSwitchTable(const Ctrl::RspSwitchTable& response);
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

    enum class OperationType
    {
        None,
        Player,
        Table
    };
    
    enum class Stage
    {
        Idle,
        AwaitingResponse,
        AwaitingErrorAcknowledgement,
        AwaitingConfirmation,
        AwaitingPick,
        AwaitingText
    };

    std::string autoOddsStatus_;    // What AutoOdds just did, for the status line

    // The workflow in progress, if it is an Operation. The others are
    // still done by the functions below, with the state above.
    std::unique_ptr<Operation> current_;
    std::unique_ptr<Operation> retired_;   // Finished; freed by the next begin()

    OperationType type_                = OperationType::None;
    Stage stage_                       = Stage::Idle;
    Ctrl::CorrelationId correlationId_ = 0;

    // Table operation in progress
    enum class TableStep { PickTable, ConfirmBets, AskBuddies };
    TableOp        tableOp_   = TableOp::List;
    TableStep      tableStep_ = TableStep::PickTable;
    std::vector<Craps::TableId> tableChoices_;   // What the pick list offered
    Craps::TableId targetTableId_;
    std::string    targetTableName_;
    bool           buddiesFollow_ = true;

    // Player operation in progress
    PlayerOp        playerOp_   = PlayerOp::List;
    Craps::PlayerId targetId_;       // Player picked, or the user player
    std::string     targetName_;
    std::string     text_;           // Name typed
    std::string     description_;    // Short description typed (Create)
    bool            askingDescription_ = false;  // Create: name given, now description

private:
    OperationManager() = default;
    bool begin(std::unique_ptr<Operation> op);
    template <class Rsp>
    void route(const char* name, const Rsp& ev,
               void (Operation::*hook)(const Rsp&));
    void finish();
    void showError(const Gen::ErrorPass& error,
                   const char* defaultType = "Betting Error",
                   const char* defaultName = "Unable to Make Bet");
    void showNotice(const std::string& brief, const std::string& detail);
    void askConfirmation(const std::string& question);
    void askForDescription();
    void askForName(const std::string& title,
                    const std::string& initial,
                    const Craps::PlayerId& exceptId);
    void sendPlayerRequest();
    void onTablePick(bool accepted, std::size_t tag);
    void afterBetsConfirmed();
    void sendTableRequest();
    const char* playerOpVerb() const;
};

} // namespace Cui

//----------------------------------------------------------------

