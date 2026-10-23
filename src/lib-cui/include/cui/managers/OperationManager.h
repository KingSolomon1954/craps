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

    /// @name Modifiers
    /// @{
    void makeBet();      // Workflow: Asynchronous bet operation
    bool confirmQuit();  // Workflow: Confirmation to quit application
    void autoFill();     // Workflow: Set up an AutoFill entry
    void quickBet();     // Workflow: Set up an QuickBet entry

    // Called only on the CUI thread by WorkOrderDispatcher / dialogs.
    void onRspMakeBet(const Ctrl::RspMakeBet& response);
    void onRspOddsBet(const Ctrl::RspOddsBet& response);
    void onConfirmationResult(SurfaceBase::OperationResult result);
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
        Quit
    };
    
    enum class Stage
    {
        Idle,
        AwaitingResponse,
        AwaitingErrorAcknowledgement,
        AwaitingConfirmation
    };

    OperationType type_                = OperationType::None;
    Stage stage_                       = Stage::Idle;
    Ctrl::CorrelationId correlationId_ = 0;

private:
    OperationManager() = default;
    void finish();
    void showError(const Gen::ErrorPass& error);
    void doContractBet(const CarrierBet& cb);
    void doOddsBet(const CarrierBet& cb);
};

} // namespace Cui

//----------------------------------------------------------------

