//----------------------------------------------------------------
//
// File: MenuBetting.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/MenuBase.h>
#include <gen/MoneyUtils.h>

namespace Cui {

class ConsoleView;        // fwd
class ScreenCrapsTable;   // fwd
class DialogAckError;     // fwd
class DialogAmountEntry;  // fwd
    
class MenuBetting : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
    MenuBetting(ConsoleView& view);
   ~MenuBetting();
    /// @}

    /// @name Modifiers
    /// @{
    void setOwningScreen(ScreenCrapsTable* pOwning);
    void onResume() override;
    /// @}

    /// @name Observers
    /// @{
    /// @}
    
protected:
    void drawMenu()            override;
    void handleMenuKey(int ch) override;

private:
    enum class ResumeState
    {
        None,
        WaitingOnPassLineAmount,
        WaitingOnComeAmount,
        WaitingOnPlace4Amount,
        WaitingOnPlace5Amount,
        WaitingOnPlace6Amount,
        WaitingOnPlace8Amount,
        WaitingOnPlace9Amount,
        WaitingOnPlace10Amount,
        WaitingOnDialogAckError
    };
    
    ScreenCrapsTable*  pOwning_;
    DialogAmountEntry* pDlgAmount_;
    DialogAckError*    pDlgError_;
    ResumeState        resumeState_          = ResumeState::None;
    ResumeState        postDialogErrorState_ = ResumeState::None;

    void setResumeState(ResumeState s);
    void doPassLine1();
    void doPassLine2();
    void doPassLineAmount(Gen::Money contractAmount);
    void doCome1();
    void doCome2();
    void doDialogAckError();
};

/*-----------------------------------------------------------*//**

@class MenuBetting

@brief Root menu for ScreeCrapsTable class

Responsibilities of MenuBetting:

@li Key bindings for the menu

@li Process input keys 

@li Takes action on input keys 

@li Renders the menu on screen

*/

} // namespace Cui

//----------------------------------------------------------------

