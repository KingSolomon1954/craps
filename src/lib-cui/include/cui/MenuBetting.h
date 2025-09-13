//----------------------------------------------------------------
//
// File: MenuBetting.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/MenuBase.h>
#include <gen/MoneyUtils.h>
#include <craps/EnumBetName.h>
#include <craps/CrapsTypes.h>

namespace Cui {

class ConsoleView;        // fwd
class ScreenCrapsTable;   // fwd
class DialogAckError;     // fwd
class DialogAmountEntry;  // fwd
class MenuPivot;          // fwd
class MenuOdds;           // fwd
    
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
        WaitingOnOddsSelection,
        WaitingOnOddsAmount,
        WaitingOnPivot,
        WaitingOnDialogAckError
    };
    
    ScreenCrapsTable*  pOwning_              = nullptr;
    DialogAmountEntry* pDlgAmount_           = nullptr;
    DialogAckError*    pDlgError_            = nullptr;
    MenuPivot*         pMenuPivot_           = nullptr;
    MenuOdds*          pMenuOdds_            = nullptr;
    ResumeState        resumeState_          = ResumeState::None;
    ResumeState        postDialogErrorState_ = ResumeState::None;
    size_t             pivot_                = 0;
    bool               isOddsBet_            = false;
    BetName            betName_;
    Craps::BetId       betId_;
    
    void showMenuPivot();
    void showMenuOdds();
    void showDialogAmountEntry();
    void showDialogAckError(const std::string& diag);
    
    void resumeBetAmount();
    void resumeOddsAmount();
    void resumeOddsSelection();
    void resumeMenuPivot();
    void resumeDialogAckError();
    void setResumeState();
    void setResumeState(ResumeState s);
    
    void doBets(BetName betName);
    void doOddsBets();
    void doMakeBet(Gen::Money contractAmount);
    void doMakeOddsBet(Gen::Money oddsAmount);
    
    void clearState();
    void setAmountTitle();
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

