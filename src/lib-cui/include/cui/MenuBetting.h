//----------------------------------------------------------------
//
// File: MenuBetting.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/MenuBase.h>
#include <gen/MoneyUtils.h>
#include <craps/EnumBetName.h>

namespace Cui {

class ConsoleView;        // fwd
class ScreenCrapsTable;   // fwd
class DialogAckError;     // fwd
class DialogAmountEntry;  // fwd
class MenuPivot;          // fwd
    
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
        WaitingOnBetAmount,
        WaitingOnOddsAmount,
        WaitingOnPivot,
        WaitingOnDialogAckError
    };
    
    ScreenCrapsTable*  pOwning_              = nullptr;
    DialogAmountEntry* pDlgAmount_           = nullptr;
    DialogAckError*    pDlgError_            = nullptr;
    MenuPivot*         pMenuPivot_           = nullptr;
    ResumeState        resumeState_          = ResumeState::None;
    ResumeState        postDialogErrorState_ = ResumeState::None;
    size_t             pivot_                = 0;
    BetName            betName_;
    
    void showMenuPivot();
    void showDialogAmountEntry();
    void showDialogAckError(const std::string& diag);
    
    void resumeBets();
    void resumeOdds();
    void resumeMenuPivot();
    void resumeDialogAckError();
    void setResumeState(ResumeState s);
    
    void doBets(BetName betName);
    void doOdds();
    void doMakeBet(Gen::Money contractAmount);
    
    void clearState();
    void setAmountPrompt();
    void setFillAmount();
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

