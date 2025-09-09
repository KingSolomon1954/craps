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
#include <cui/DialogAmountEntry.h>

using namespace Cui;

//----------------------------------------------------------------

MenuBetting::MenuBetting(ConsoleView& view)
    : MenuBase(view, 5, 6)  // TODO Howie need layout
{
    // Obtain the DialogEntryAmount screen and cache it
    auto* pScreen = view_.getScreen(ConsoleView::ScreenId::DialogAmountEntry);
    auto* pDialogAe_ = dynamic_cast<DialogAmountEntry*>(pScreen);
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
    case ResumeState::None:                    assert(false); break;
    case ResumeState::WaitingOnPassLineAmount: doPassLine2(); break;
    case ResumeState::WaitingOnComeAmount:     doCome2();     break;
    }
}

//----------------------------------------------------------------

void
MenuBetting::doPassLine1()
{
    pDialogAe_->setPrompt("PassLine Bet");
    pDialogAe_->setFillAmount(100);  // TODO: find correct fill amount
    setResumeState(ResumeState::WaitingOnPassLineAmount);
    view_.pushScreen(pDialogAe_);
}

//----------------------------------------------------------------

void
MenuBetting::doPassLine2()
{
    auto rs = pDialogAe_->getResults();
    
    if (rs.canceled)
    {
        // empty
    }
    else
    {
        doPassLineAmount(rs.amount);
    }
        
    setResumeState(ResumeState::None);
    view_.popScreen();
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
#if 0  // TODO
        // Show diagnostic dialog overlay
        view_.pushScreen(ConsoleView::ScreenId::DialogDiagnostic);

        auto pDd = dynamic_cast<DialogDiagnostic*>(
            view_.getScreen(ConsoleView::ScreenId::DialogDiagnostic));
        if (pDd)
        {
            pDd->setMessage(ep.message);
        }
#endif
    }
    else
    {
#if 0  // TODO
        // Bet succeeded — notify owning screen to update visuals
        if (pOwning_)
        {
            pOwning_->drawMakeBet(playerId_, betId);
        }
#endif
    }
}

//----------------------------------------------------------------

void
MenuBetting::doCome1()
{
    
}

//----------------------------------------------------------------

void
MenuBetting::doCome2()
{
    
}

//----------------------------------------------------------------

void
MenuBetting::setResumeState(ResumeState s)
{
    resumeState_ = s;
}

//----------------------------------------------------------------

