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
    static OperationManager& instance();

    OperationManager(const OperationManager&) = delete;
    OperationManager& operator=(const OperationManager&) = delete;
    OperationManager(OperationManager&&) = delete;
    OperationManager& operator=(OperationManager&&) = delete;

    // Starts an asynchronous bet operation using the completed CarrierBet.
    //
    void makeBet();

    // Starts the confirmation workflow for quitting the application.
    bool confirmQuit();

    // Called only on the CUI thread by WorkOrderDispatcher / dialogs.
    void onMakeBetResponse(const Ctrl::RspMakeBet& response);
    void onConfirmationResult(SurfaceBase::OperationResult result);
    void onErrorAcknowledged();

    bool isActive() const;

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

