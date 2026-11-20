//----------------------------------------------------------------
//
// File: OperationManager.h
//
//----------------------------------------------------------------

#pragma once

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
    bool chooseAnimationSpeed(); // Workflow: Pick the dice animation speed
    bool chooseRollHistoryFormat(); // Workflow: Pick how rolls are listed
    bool confirmQuit();  // Workflow: Confirmation to quit application
    void autoFill();     // Workflow: Set up an AutoFill entry
    void quickBet();     // Workflow: Set up an QuickBet entry

    // Called only on the CUI thread by WorkOrderDispatcher / dialogs.
    void onRspMakeBet(const Ctrl::RspMakeBet& response);
    void onRspOddsBet(const Ctrl::RspOddsBet& response);
    void onRspRemoveBet(const Ctrl::RspRemoveBet& response);
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
    /// @}

private:
    enum class OperationType
    {
        None,
        MakeBet,
        RemoveBet,
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

