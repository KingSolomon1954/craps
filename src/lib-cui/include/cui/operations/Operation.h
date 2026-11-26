//----------------------------------------------------------------
//
// File: Operation.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/GameEvents.h>
#include <cui/bases/SurfaceBase.h>
#include <cui/dialogs/DialogPicker.h>
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
    virtual void onConfirm(SurfaceBase::OperationResult result);

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

    /// Ask a yes or no question and wait for the answer in onConfirm().
    void askConfirmation(const std::string& question);
};

} // namespace Cui

//----------------------------------------------------------------
