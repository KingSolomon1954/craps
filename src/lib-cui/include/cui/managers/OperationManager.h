//----------------------------------------------------------------
//
// File: OperationManager.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/AutoOdds.h>
#include <controller/BetSpec.h>
#include <controller/GameEvents.h>
#include <cui/bases/SurfaceBase.h>
#include <cui/managers/CarrierBet.h>
#include <gen/ErrorPass.h>
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

    /// @name Modifiers
    /// @{
    bool playerOperation(PlayerOp op);  // Workflow: Player menu operations
    void makeBet();      // Workflow: Asynchronous bet operation
    bool removeBets();   // Workflow: Pick one of your bets to take down
    bool undoLastBet();  // Workflow: Take back your latest bet change
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
    enum class OperationType
    {
        None,
        MakeBet,
        RemoveBet,
        UndoBet,
        QuickBets,
        QuickBetsEdit,
        QuickBetsManage,
        AutoOdds,
        Quit,
        Player,
        AnimationSpeed,
        RollHistoryFormat
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

    // Taking bets down: what the pick list offered, and what is left to do
    struct Removal
    {
        Craps::BetId betId    = 0;
        bool         oddsOnly = false;   // Contract bet stays
    };
    std::vector<Removal> removalChoices_;
    std::vector<Removal> removalQueue_;   // Sent one at a time
    bool removalStarted_ = false;         // Later removals join the first's undo step

    std::vector<std::string> quickBetNames_;   // What the pick list offered
    Ctrl::BetSpec            quickBetSpec_;    // The bet being saved
    std::string              quickBetTarget_;  // Entry to add to, or new name

    // Managing QuickBets: one pick list or prompt per step
    enum class ManageStep { PickEntry, PickAction, PickBet, AskName, AskAmount,
                            ConfirmDelete };
    ManageStep               manageStep_ = ManageStep::PickEntry;
    Ctrl::ReqEditQuickBets::Action manageAction_ = Ctrl::ReqEditQuickBets::Action::Rename;
    std::string              manageEntry_;     // Entry being managed
    std::size_t              manageBetIndex_ = 0;

    // AutoOdds, from the Configuration menu: one pick list or prompt per step
    enum class AutoStep { PickAction, PickType, PickPoint, AskMultiple,
                          ConfirmClear };
    AutoStep                 autoStep_ = AutoStep::PickAction;
    BetName                  autoBetName_ = BetName::Invalid;
    unsigned                 autoPoint_   = 0;       // 0 means all points
    bool                     autoTurnOn_  = false;
    std::string              autoOddsStatus_;

    OperationType type_                = OperationType::None;
    Stage stage_                       = Stage::Idle;
    Ctrl::CorrelationId correlationId_ = 0;

    // Player operation in progress
    PlayerOp        playerOp_   = PlayerOp::List;
    Craps::PlayerId targetId_;       // Player picked, or the user player
    std::string     targetName_;
    std::string     text_;           // Name typed
    std::string     description_;    // Short description typed (Create)
    bool            askingDescription_ = false;  // Create: name given, now description

private:
    OperationManager() = default;
    void finish();
    void showError(const Gen::ErrorPass& error,
                   const char* defaultType = "Betting Error",
                   const char* defaultName = "Unable to Make Bet");
    void showNotice(const std::string& brief, const std::string& detail);
    void askConfirmation(const std::string& question);
    void askForDescription();
    void askForQuickBetsName();
    void sendQuickBetsEdit(Ctrl::ReqEditQuickBets::Action action);
    void sendManageRequest(const std::string& text = std::string());
    void onManagePick(bool accepted, std::size_t tag);
    void pickManageAction();
    void pickManageBet(Ctrl::ReqEditQuickBets::Action action);
    void onAutoPick(bool accepted, std::size_t tag);
    void pickAutoAction();
    void pickAutoType();
    void pickAutoPoint();
    void askAutoMultiple();
    void sendAutoRequest(Ctrl::ReqEditAutoOdds request);
    void askManageName();
    void askManageAmount();
    void askForName(const std::string& title,
                    const std::string& initial,
                    const Craps::PlayerId& exceptId);
    void sendPlayerRequest();
    const char* playerOpVerb() const;
    void sendNextRemoval();
    void onRemovalResult(bool accepted, const Gen::ErrorPass& error);
    void doContractBet(const CarrierBet& cb);
    void doOddsBet(const CarrierBet& cb);
};

} // namespace Cui

//----------------------------------------------------------------

