//----------------------------------------------------------------
//
// File: MenuBetting.cpp
//
//----------------------------------------------------------------

#include <cui/MenuBetting.h>

#include <cassert>
#include <controller/CrapsCommands.h>
#include <controller/CrapsReaders.h>
#include <craps/EnumBetName.h>
#include <cui/ConsoleView.h>
#include <cui/DialogAckError.h>
#include <cui/DialogAmountEntry.h>
#include <cui/ScreenCrapsTable.h>
#include <cui/MenuPivot.h>
#include <cui/MenuOdds.h>

using namespace Cui;

//----------------------------------------------------------------

MenuBetting::MenuBetting()
    : MenuBase(1, 1)  // Placeholder, resized in drawMenu()
{
    // Obtain common menus/dialog screens and cache them
    auto* pDlgAmount_ = DialogAmountEntry::instance();
    auto* pMenuPivot_ = MenuPivot::instance();
    auto* pMenuOdds_  = MenuOdds::instance();
    auto* pDlgError_  = DialogAckError::instance();
}

//----------------------------------------------------------------

MenuBetting*
MenuBetting::instance()
{
    static MenuBetting menuBetting;
    return &menuBetting;
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
    case 'd': doBets(BetName::DontPass); break;
    case 'k': doBets(BetName::DontCome); break;
    case 'l': doBets(BetName::Place);    break;
    case 'h': doBets(BetName::Hardway);  break;
    case 'f': doBets(BetName::Field);    break;
    case 'e': doBets(BetName::CandE);    break;
    case 'a': doBets(BetName::AnyCraps); break;
    case '7': doBets(BetName::AnySeven); break;
    case 'n': doBets(BetName::Horn);     break;
    case 'o': doOddsBets();              break;
    // TODO add more...
    // case 'x': doRemoveBet();          break;
    // case 't': doBetFlags();           break;
    // case 'q': doQuickBet();           break;
    // case 'r': doRollDice();           break;
    // case 'u': doUndoLastBet();        break;
    // case '.': doControlMenu();        break;
    // case 'v': doViewMenu();           break;
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
    case BetName::DontCome:
    case BetName::Field:
    case BetName::CandE:
    case BetName::AnyCraps:
    case BetName::AnySeven:
    case BetName::Horn:      showDialogAmountEntry(); break;
    case BetName::Place:     
    case BetName::Hardway:   showMenuPivot();         break;
    }
}

//----------------------------------------------------------------

void
MenuBetting::doOddsBets()
{
    showMenuOdds();
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
    case ResumeState::WaitingOnBetAmount:      resumeBetAmount();      break;
    case ResumeState::WaitingOnOddsAmount:     resumeOddsAmount();     break;
    case ResumeState::WaitingOnOddsSelection:  resumeOddsSelection();  break;
    case ResumeState::WaitingOnPivot:          resumeMenuPivot();      break;
    case ResumeState::WaitingOnDialogAckError: resumeDialogAckError(); break;
    }
}

//----------------------------------------------------------------

void
MenuBetting::resumeBetAmount()
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
MenuBetting::resumeOddsAmount()
{
    // Back from DialogAmountEntry
    auto rs = pDlgAmount_->getResults();
    
    if (rs.canceled)
    {
        setResumeState(ResumeState::None);
        view_.popScreen();
        return;
    }

    doMakeOddsBet(rs.amount);
}

//----------------------------------------------------------------

void
MenuBetting::resumeOddsSelection()
{
    // Back from MenuOdds, now we know which bet to set odds on
    auto rs = pMenuOdds_->getResults();

    if (rs.canceled)
    {
        setResumeState(ResumeState::None);
        view_.popScreen();
        return;
    }
    
    betName_   = rs.betName;  // PassLine, Come, DontPass, DontCome
    betId_     = rs.betId;
    pivot_     = rs.pivot;    // 4,5,6,8,9,10
    isOddsBet_ = true;
    
    showDialogAmountEntry();
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
    auto rc = Ctrl::CrapsReaders::getUserPlayer(playerId, ep);
    assert(rc == Gen::ReturnCode::Success);

    // Make the bet
    auto correlationId = Ctrl::CrapsCommands::cmdMakeBet(
        playerId,
        betName_,
        contractAmount,
        pivot_);

// TODO setup to listen for Pub/Sub event
// EventType::ViewMakeBetSuccess or
// TODO setup to listen for ViewErrorDialog
    
    // Bet succeeded
    pOwning_->onBetPlaced(playerId, betId);  // Update visuals
    setResumeState(ResumeState::None);
    view_.popScreen();
}

//----------------------------------------------------------------

void
MenuBetting::doMakeOddsBet(Gen::Money oddsAmount)
{
    Gen::ErrorPass  ep;
    Craps::PlayerId playerId;
    auto rc = Ctrl::CrapsReaders::getUserPlayer(playerId, ep);
    assert(rc == Gen::ReturnCode::Success);

    // Set or change the odds bet
    auto correlationId = Ctrl::CrapsCommands::cmdBetSetOddsAmount(
        betId_,
        oddsAmount);

// TODO setup to listen for Pub/Sub event
// EventType::ViewMakeBetSuccess or
// TODO setup to listen for ViewErrorDialog
    
    // Bet succeeded
    pOwning_->onBetPlaced(playerId, betId_);  // Update visuals
    setResumeState(ResumeState::None);
    view_.popScreen();
}

//----------------------------------------------------------------

void
MenuBetting::showDialogAmountEntry()
{
    // Bring up DialogAmountEntry
    pDlgAmount_->clearState();
    setAmountTitle();
    setFillAmount();
    setQuickBet();
    setResumeState();
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
MenuBetting::showMenuOdds()
{
    // Bring up next level menu to present choices for odds bets
    pMenuOdds_->clearState();
    setResumeState(ResumeState::WaitingOnOddsSelection);
    view_.pushScreen(pMenuOdds_);
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
//
// Forming titles like this:
//
//     "PassLine Bet"
//     "Place Bet on 6"
//     "Hardway Bet on 6"
//     "Odds on PassLine 6"
//     "Odds on Come 6"
//     "Odds on DontPass 6"
//     "Odds on DontCome 6"
//    
void
MenuBetting::setAmountTitle()
{
    std::string betName  = EnumBetName::toString(betName_);
    std::string pivotStr = std::to_string(pivot_);
    std::string title;

    if (isOddsBet_)
    {
        title = "Odds on " + betName + " " + pivotStr;
    }
    else
    {
        title = betName + " Bet";
        if (pivot_ != 0)
        {
            title += " on " + pivotStr;
        }
    }
    pDlgAmount_->setTitle(title);
}

//----------------------------------------------------------------

void
MenuBetting::setFillAmount()
{
    pDlgAmount_->registerAutoFillCallback(
        [this](Gen::Money amount) { autoFillCallback(amount); }
    );

    Gen::Money fillAmount = 0;
    Gen::ErrorPass ep;
    Ctrl::CrapsReaders::AutoFillEntry afe =
        {betName_, pivot_, isOddsBet_, 0};
    auto rc = Ctrl::CrapsReaders::getAutoFill(afe, ep);
    if (rc == Gen::ReturnCode::Success)
    {
        fillAmount = afe.amount;
    }
    else
    {
        // TODO 
        // fillAmount = tableMinimum(betName_);
    }

    pDlgAmount_->setFillAmount(fillAmount);
}

//----------------------------------------------------------------

void
MenuBetting::setQuickBet()
{
    pDlgAmount_->registerQuickBetCallback(
        [this](Gen::Money amount) { quickBetCallback(amount); }
    );
}

//----------------------------------------------------------------

void
MenuBetting::setResumeState(ResumeState s)
{
    resumeState_ = s;
}

//----------------------------------------------------------------

void
MenuBetting::setResumeState()
{
    if (isOddsBet_)
    {
        setResumeState(ResumeState::WaitingOnOddsAmount);
    }
    else
    {
        setResumeState(ResumeState::WaitingOnBetAmount);
    }
}

//----------------------------------------------------------------
//
// The DialogAmountEntry calls this when user presses
// 'a' to create an auto fill entry using the current bet and amount.
//
void
MenuBetting::autoFillCallback(Gen::Money amount)
{
    Gen::ErrorPass ep;
    Ctrl::CrapsCommands::AutoFillEntry afe =
        {betName_, pivot_, isOddsBet_, amount};

    if (amount == 0)
    {
        Ctrl::CrapsCommands::cmdDeleteAutoFill(afe, ep);
        return;
    }
    Ctrl::CrapsCommands::cmdSetAutoFill(afe, ep);
}

//----------------------------------------------------------------
//
// The DialogAmountEntry calls this when user presses
// 'q' to set quick bet using the current bet and amount.
//
void
MenuBetting::quickBetCallback(Gen::Money amount)
{
    Gen::ErrorPass ep;
    Ctrl::CrapsCommands::QuickBetEntry qbe =
        {betName_, pivot_, isOddsBet_, amount};
    Ctrl::CrapsCommands::cmdSetQuickBet(qbe, ep);
}

//----------------------------------------------------------------

void
MenuBetting::clearState()
{
    pivot_                = 0;
    betName_              = BetName::Invalid;
    betId_                = 0;
    resumeState_          = ResumeState::None;
    postDialogErrorState_ = ResumeState::None;
    isOddsBet_            = false;
}    

//----------------------------------------------------------------

