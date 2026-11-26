//----------------------------------------------------------------
//
// File: Operation.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/GameEvents.h>
#include <cui/bases/SurfaceBase.h>
#include <cui/dialogs/DialogPicker.h>
#include <cui/dialogs/DialogTextInput.h>
#include <gen/ErrorPass.h>
#include <string>
#include <vector>

namespace Cui {

/*-----------------------------------------------------------*//**

@class Operation

@brief One workflow the person can start: a short chain of dialogs,
       usually ending with a request to the Controller.

An Operation is made by OperationManager when the person starts a
workflow, lives until the workflow finishes, and is then discarded. All
of a workflow's state is in its own object, so there is nothing to reset.

OperationManager forwards what happens (a dialog answered, an error
acknowledged) to the current Operation. Each of the hooks below does
nothing useful by default. An Operation overrides the ones its workflow
can receive.

An Operation does not decide what is shown next from outside. It says so
itself, by calling one of the protected helpers (pickFrom(),
askConfirmation()) or by finishing.

Everything runs on the CUI thread.

*/
class Operation
{
public:
    virtual ~Operation() = default;

    /// Begin the workflow: push the first dialog.
    virtual void start() = 0;

    /// @name Hooks
    /// Called when the dialog this Operation pushed has been answered.
    /// @{
    virtual void onPick(bool accepted,
                        const Craps::PlayerId& playerId,
                        Craps::BetId betId,
                        std::size_t tag);
    virtual void onText(bool accepted, const std::string& text);

    /// The answer of DialogQuickBetsSetAdd: an entry's index, or
    /// DialogQuickBetsSetAdd::NewEntry.
    virtual void onQuickBetsChoice(bool accepted, std::size_t index);
    virtual void onConfirm(SurfaceBase::OperationResult result);

    /// The Controller's answer to the request this Operation sent. Only
    /// an answer with the right correlation id gets here. An Operation
    /// overrides the ones it can receive.
    virtual void onRspMakeBet(const Ctrl::RspMakeBet& rsp);
    virtual void onRspOddsBet(const Ctrl::RspOddsBet& rsp);
    virtual void onRspRemoveBet(const Ctrl::RspRemoveBet& rsp);
    virtual void onRspUndoBet(const Ctrl::RspUndoBet& rsp);
    virtual void onRspSetBetFlags(const Ctrl::RspSetBetFlags& rsp);
    virtual void onRspApplyQuickBets(const Ctrl::RspApplyQuickBets& rsp);
    virtual void onRspEditQuickBets(const Ctrl::RspEditQuickBets& rsp);
    virtual void onRspEditAutoOdds(const Ctrl::RspEditAutoOdds& rsp);

    /// The error dialog has closed. By default, unwind to the root screen
    /// and finish.
    virtual void onErrorAcknowledged();
    /// @}

protected:
    Operation() = default;

    /// The workflow is over. The Operation is discarded after the hook
    /// that called this returns.
    void finish();

    /// Show a pick list and wait for the answer in onPick().
    void pickFrom(const std::string& title,
                  std::vector<DialogPicker::Item> items);

    /// A request has been sent: wait for the answer with this correlation
    /// id in one of the onRsp hooks.
    void awaitResponse(Ctrl::CorrelationId id);

    /// Show an error dialog and wait for it to be acknowledged. The labels
    /// are used when the error has none of its own.
    void showError(const Gen::ErrorPass& error,
                   const char* defaultType = "Betting Error",
                   const char* defaultName = "Unable to Make Bet");

    /// Show a notice. It does not wait for anything.
    void showNotice(const std::string& brief, const std::string& detail);

    /// Unwind the menus and dialogs to the table, as after any bet, and
    /// finish.
    void finishAtRoot();

    /// Show a text prompt and wait for the answer in onText(). The
    /// validator returns why the text is refused, or nothing if it is fine.
    void askText(const std::string& title,
                 const std::string& prompt,
                 const std::string& initial,
                 std::size_t maxChars,
                 DialogTextInput::Validator validator);

    /// For an Operation that pushes a dialog of its own: wait for its
    /// answer in onPick() or onQuickBetsChoice().
    void awaitPick();

    /// Ask a yes or no question and wait for the answer in onConfirm().
    void askConfirmation(const std::string& question);
};

} // namespace Cui

//----------------------------------------------------------------
