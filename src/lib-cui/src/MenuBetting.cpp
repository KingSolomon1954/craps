//----------------------------------------------------------------
//
// File: MenuBetting.cpp
//
//----------------------------------------------------------------

#include <cui/MenuBetting.h>

#include <cassert>
#include <controller/CrapsInterface.h>
#include <cui/ConsoleView.h>
#include <cui/ScreenCrapsTable.h>
#include <cui/DialogAckError.h>
#include <cui/DialogAmountEntry.h>

using namespace Cui;

//----------------------------------------------------------------

MenuBetting::MenuBetting(ConsoleView& view)
    : MenuBase(view, 5, 6)  // TODO Howie need layout
{
    // Obtain the DialogAmountEntry screen and cache it
    auto* pDlgAmount_ = dynamic_cast<DialogAmountEntry*>(
        view_.getScreen(ConsoleView::ScreenId::DialogAmountEntry));
    // Obtain the DialogAckError screen and cache it
    auto* pDlgError_ = dynamic_cast<DialogAckError*>(
        view_.getScreen(ConsoleView::ScreenId::DialogAckError));
}

//----------------------------------------------------------------

MenuBetting::~MenuBetting()
{
}

//----------------------------------------------------------------

void
MenuBetting::setOwningScreen(ScreenCrapsTable* pOwning)
{
    pOwning_ = pOwning;
}

//----------------------------------------------------------------
//
// Override menu base class
//
void
MenuBetting::drawMenu()
{
    // TODO
    mvwprintw(w_, 0, 0, "Betting Menu:");
    mvwprintw(w_, 1, 2, "1) Pass Line Bet");
    mvwprintw(w_, 2, 2, "2) Don't Pass");
    // etc.
}

//----------------------------------------------------------------
//
// Override menu base class
//
void
MenuBetting::handleMenuKey(int ch)
{
    clearState();
    switch (ch)
    {
    case 'p': doBets(BetName::PassLine); break;
    case 'c': doBets(BetName::Come);     break;
    case 'o': doOdds();                  break;
    }
}

//----------------------------------------------------------------

void
MenuBetting::doBets(BetName betName)
{
    betName_ = betName;
    switch(betName_)
    {
    case BetName::PassLine:
    case BetName::Come:
    case BetName::DontPass:
    case BetName::DontCome:  showDialogAmountEntry(); break;
    case BetName::Place:     
    case BetName::Hardway:   showMenuPivot();      break;
    }
}

//----------------------------------------------------------------

void
MenuBetting::doOdds()
{
}

//----------------------------------------------------------------
//
// Override Screen base class
// Called when MenuBetting regains focus
// In most cases DialogAmountEntry screen finished
//
void
MenuBetting::onResume()
{
    switch (resumeState_)
    {
    case ResumeState::None:                    /* do nothing */        break;
    case ResumeState::WaitingOnBetAmount:      resumeBets();           break;
    case ResumeState::WaitingOnOddsAmount:     resumeOdds();           break;
    case ResumeState::WaitingOnPivot:          resumeMenuPivot();      break;
    case ResumeState::WaitingOnDialogAckError: resumeDialogAckError(); break;
    }
}

//----------------------------------------------------------------

void
MenuBetting::resumeBets()
{
    // Back from DialogAmountEntry
    auto rs = pDlgAmount_->getResults();
    
    if (rs.canceled)
    {
        setResumeState(ResumeState::None);
        view_.popScreen();
        return;
    }
    doMakeBet(rs.amount);
}

//----------------------------------------------------------------

void
MenuBetting::resumeOdds()
{
    // Back from DialogAmountEntry
    // TODO
}

//----------------------------------------------------------------

void
MenuBetting::resumeMenuPivot()
{
    // Back from MenuPivot 
    auto rs = pMenuPivot_->getResults();

    if (rs.canceled)
    {
        setResumeState(ResumeState::None);
        view_.popScreen();
        return;
    }
    
    pivot_ = rs.pivot;
    showDialogAmountEntry();
}

//----------------------------------------------------------------

void
MenuBetting::resumeDialogAckError()
{
    // Error dialog was dismissed, restore "return-to" state
    resumeState_ = postDialogErrorState_;
    postDialogErrorState_ = ResumeState::None;
}
        
//----------------------------------------------------------------

void
MenuBetting::doMakeBet(Gen::Money contractAmount)
{
    Gen::ErrorPass  ep;
    Craps::BetId    betId;
    Craps::PlayerId playerId;

    // Grab playerId
    auto rc = Ctrl::CrapsInterface::getUserPlayer(playerId, ep);
    assert(rc == Gen::ReturnCode::Success);

    // Make the bet
    rc = Ctrl::CrapsInterface::playerMakeBet(
        playerId,
        betName_,
        contractAmount,
        pivot_, // pivot
        betId,  // return value
        ep);
    if (rc == Gen::ReturnCode::Fail)
    {
        pOwning_->onBetFailed(playerId, ep.diag);
        showDialogAckError(ep.diag);
        return;
    }
    
    // Bet succeeded
    pOwning_->onBetPlaced(playerId, betId);  // Update visuals
    setResumeState(ResumeState::None);
    view_.popScreen();
}

//----------------------------------------------------------------

void
MenuBetting::showDialogAmountEntry()
{
    // Bring up DialogAmountEntry
    setAmountPrompt();
    setFillAmount();
    setResumeState(ResumeState::WaitingOnBetAmount);
    view_.pushScreen(pDlgAmount_);
}

//----------------------------------------------------------------

void
MenuBetting::showMenuPivot()
{
    // Bring up next level menu to obtain pivot
    assert(betName_ == BetName::Place || betName_ == BetName::Hardway);
    pMenuPivot_->setUpFor(betName_);
    setResumeState(ResumeState::WaitingOnPivot);
    view_.pushScreen(pMenuPivot_);
}

//----------------------------------------------------------------

void
MenuBetting::showDialogAckError(const std::string& diag)
{
    pDlgError_->setMessage(diag);
    postDialogErrorState_ = resumeState_;
    setResumeState(ResumeState::WaitingOnDialogAckError);
    view_.pushScreen(pDlgError_);
}

//----------------------------------------------------------------

void
MenuBetting::setResumeState(ResumeState s)
{
    resumeState_ = s;
}

//----------------------------------------------------------------

void
MenuBetting::setAmountPrompt()
{
    // TODO
    // "Place Bet on 6"
    std::string s = "Place Bet on 6";
    pDlgAmount_->setPrompt(s);
}

//----------------------------------------------------------------

void
MenuBetting::setFillAmount()
{
    // TODO init to table minimum
    Gen::Money fillAmount = 0;
    Gen::ErrorPass ep;
        
    // TODO
    // lookup CrapsInterface::getFillAmount(betName_, pivot_, fillAmoutn, ep);
    pDlgAmount_->setFillAmount(fillAmount);
}

//----------------------------------------------------------------

void
MenuBetting::clearState()
{
    pivot_                = 0;
    betName_              = BetName::Invalid;
    resumeState_          = ResumeState::None;
    postDialogErrorState_ = ResumeState::None;
}    

//----------------------------------------------------------------

