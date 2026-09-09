//----------------------------------------------------------------
//
// File: DialogConfirm.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/DialogBase.h>
#include <string>

//----------------------------------------------------------------

namespace Cui {

class DialogConfirm : public DialogBase
{
public:
    static DialogConfirm& instance();

    DialogConfirm(const DialogConfirm&)            = delete;
    DialogConfirm& operator=(const DialogConfirm&) = delete;
    DialogConfirm(DialogConfirm&&)                 = delete;
    DialogConfirm& operator=(DialogConfirm&&)      = delete;

    void draw() override;
    bool handleKey(int ch) override;
    void onAttach(SurfaceBase* pParent) override;

    void configure(const std::string& message);
    
private:
    DialogConfirm();

    void resizeWindow();
    void drawBorders();
    void drawStaticContent();
    void populate();
    void processSelection(OperationResult r);

private:
    std::string text_;    // title bar message
    int winBorderTopCol_ = 20;  // TODO from CUI window placement mgr
    int winBorderTopRow_ = 10;  // TODO from CUI window placement mgr
};

/*-----------------------------------------------------------*//**

@class DialogConfirm

@brief Confirmation dialog for yes/no/cancel operations.

The dialog accepts:

@li Y/y - Yes
@li N/n - No
@li Esc - Cancel

The dialog is configured with a caller-supplied message, for example:

@code
DialogConfirm::instance().configure(
    "Confirm Quit RoyalCraps game");
@endcode

The result is available through getResult() and is propagated
through the parent surface chain when the dialog closes.

*/

} // namespace Cui

//----------------------------------------------------------------






