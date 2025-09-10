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
    switch (ch)
    {
    case 'p': doPassLine1(); break;
    case 'c': doCome1();     break;
    }
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
    case ResumeState::None:                    assert(false);      break;
    case ResumeState::WaitingOnPassLineAmount: doPassLine2();      break;
    case ResumeState::WaitingOnComeAmount:     doCome2();          break;
    case ResumeState::WaitingOnDialogAckError: doDialogAckError(); break;
    }
}

//----------------------------------------------------------------

void
MenuBetting::doPassLine1()
{
    // Setup to display DialogAmountEntry
    pDlgAmount_->setPrompt("PassLine Bet");
    pDlgAmount_->setFillAmount(100);  // TODO: find correct fill amount
    setResumeState(ResumeState::WaitingOnPassLineAmount);
    view_.pushScreen(pDlgAmount_);
}

//----------------------------------------------------------------

void
MenuBetting::doPassLine2()
{
    // Back from DialogAmountEntry
    auto rs = pDlgAmount_->getResults();
    
    if (rs.canceled)
    {
        // Empty
    }
    else
    {
        doPassLineAmount(rs.amount);
    }
}

//----------------------------------------------------------------

void
MenuBetting::doPassLineAmount(Gen::Money contractAmount)
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
        BetName::PassLine,
        contractAmount,
        0,      // pivot
        betId,  // return value
        ep);
    if (rc == Gen::ReturnCode::Fail)
    {
        pOwning_->onBetFailed(playerId, ep.diag);        
        pDlgError_->setMessage(ep.diag);
        postDialogErrorState_ = resumeState_;
        setResumeState(ResumeState::WaitingOnDialogAckError);
        view_.pushScreen(pDlgError_);
        return;
    }
    
    // Bet succeeded
    pOwning_->onBetPlaced(playerId, betId);  // Update visuals
    setResumeState(ResumeState::None);
    view_.popScreen();
}

//----------------------------------------------------------------

void
MenuBetting::doCome1()
{
    // TODO    
}

//----------------------------------------------------------------

void
MenuBetting::doCome2()
{
    // TODO    
}

//----------------------------------------------------------------

void
MenuBetting::doDialogAckError()
{
    // Error dialog was dismissed, restore "return-to" state
    resumeState_ = postDialogErrorState_;
    postDialogErrorState_ = ResumeState::None;
}
        
//----------------------------------------------------------------

void
MenuBetting::setResumeState(ResumeState s)
{
    resumeState_ = s;
}

//----------------------------------------------------------------

