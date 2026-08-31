//----------------------------------------------------------------
//
// File: MenuBetting.cpp
//
//----------------------------------------------------------------

#include <cui/MenuBetting.h>

#include <cui/CarrierBet.h>
#include <cui/ConsoleManager.h>
#include <cui/CuiUtils.h>
#include <cui/WindowNavBar.h>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

MenuBetting::MenuBetting()
{
    createWindow();
    fillWindow();
}

//----------------------------------------------------------------

MenuBetting&
MenuBetting::instance()
{
    static MenuBetting menu;
    return menu;
}

//----------------------------------------------------------------

void
MenuBetting::createWindow()
{
    using L = Layout;
    newWindow(L::height,    L::width,
              L::winStartY, L::winStartX,
              "MenuBetting");
}

//----------------------------------------------------------------
//
// Fills in the window like this, just this once at init time.
//
// Later, multiple calls to draw() just transfers the already
// filled window.
//
//    0123456789012345678901234
// 0  ┌───────────────────────┐
// 1  │ Betting Menu          │
// 2  ├───────────────────────┤
// 3  │ [p] Pass Line         │
// 4  │ [c] Come Bet          │
// 5  │ [d] Dont Pass         │
// 6  │ [k] Dont Come         │
// 7  │ [o] Odds Bet          │
// 8  │ [l] Place Bet         │
// 9  │ [h] Hardways          │
// 10 │ [f] Field Bet         │
// 11 │ [e] C&E Bet           │
// 12 │ [a] Any Craps         │
// 13 │ [7] Any 7             │
// 14 │ [n] Horn Bet          │
// 15 │ [w] World Bet         │
// 16 │ [g] Bet Flags         │
// 17 │ [q] Quick Bet         │
// 18 │ [x] Remove Bets       │
// 19 │ [u] Undo Last         │
// 20 │ [r] Roll Dice         │
// 21 │ [. or esc] Back       │
// 22 └───────────────────────┘
//
void
MenuBetting::fillWindow()
{
    using L = Layout;

    box(pWin_, 0, 0);

    // Horizontal separator below the title
    mvwhline(pWin_, 2, 1, ACS_HLINE, L::width - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, L::width - 1, ACS_RTEE);

    mvwaddstr(pWin_, 1, 2, "Betting Menu");

    mvwaddstr(pWin_, 3,  2, "[p] Pass Line");
    mvwaddstr(pWin_, 4,  2, "[c] Come Bet");
    mvwaddstr(pWin_, 5,  2, "[d] Dont Pass");
    mvwaddstr(pWin_, 6,  2, "[k] Dont Come");
    mvwaddstr(pWin_, 7,  2, "[o] Odds Bet");
    mvwaddstr(pWin_, 8,  2, "[l] Place Bet");
    mvwaddstr(pWin_, 9,  2, "[h] Hardways");
    mvwaddstr(pWin_, 10, 2, "[f] Field Bet");
    mvwaddstr(pWin_, 11, 2, "[e] C&E Bet");
    mvwaddstr(pWin_, 12, 2, "[a] Any Craps");
    mvwaddstr(pWin_, 13, 2, "[7] Any 7");
    mvwaddstr(pWin_, 14, 2, "[n] Horn Bet");
    mvwaddstr(pWin_, 15, 2, "[w] World Bet (todo)");
    mvwaddstr(pWin_, 16, 2, "[g] Bet Flags");
    mvwaddstr(pWin_, 17, 2, "[q] Quick Bet");
    mvwaddstr(pWin_, 18, 2, "[x] Remove Bets");
    mvwaddstr(pWin_, 19, 2, "[u] Undo Last");
    mvwaddstr(pWin_, 20, 2, "[r] Roll Dice");
    mvwaddstr(pWin_, 21, 2, "[esc] Back");
}

//----------------------------------------------------------------

void
MenuBetting::draw()
{
    // Just reuse already filled window over and over
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------
//
// Override menu base class
//
void
MenuBetting::handleKey(int ch)
{
    betName_ = BetName::Invalid;

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
//  case 'w': doBets(BetName::World);    break;
    case 'o': doOddsBets();              break;
    case 'g': doBetFlags();              break;
    case 'x': doRemoveBets();            break;
    case 'u': doUndoLastBet();           break;
    case 'q': doQuickBet();              break;
    case 'r': doRollDice();              break;
    case '.':
    case  27: back();                    break;
    default: WindowNavBar::instance().handleKey(ch); break;
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
    // case BetName::World:
    case BetName::Horn:      doGetAmount(); break;
    case BetName::Place:
    case BetName::Hardway:   doGetPivot();  break;
    default: assert(true);
             throw std::runtime_error("MenuBetting::doBets() "
                 "missing case block for bet name: " +
                 EnumBetName::toString(betName_));
             break;
    }
}

//----------------------------------------------------------------

void
MenuBetting::doGetAmount()
{
    populateCarrier();
    prepDialogAmount();
    activateDialogAmount();
}

//----------------------------------------------------------------

void
MenuBetting::doGetPivot()
{
    populateCarrier();
    activateMenuPivot();
}

//----------------------------------------------------------------

void
MenuBetting::populateCarrier()
{
    auto cb = CarrierBet::instance();
    cb.clear();
    cb.setBetType(betName_);
}

//----------------------------------------------------------------

void
MenuBetting::prepDialogAmount()
{
    // TODO
    // DialogBetAmount::setPrompt("Amount for %s" bet, EnumBetName::toString(betName_));
    // auto amount = getAutoFillAmount(betName_);
    // DialogBetAmount::preFill(amount);
}

//----------------------------------------------------------------

void
MenuBetting::activateDialogAmount()
{
    // TODO
    // ConsoleManager::pushSurface(AmountDialog);
}

//----------------------------------------------------------------

void
MenuBetting::activateMenuPivot()
{
    // TODO
    // ConsoleManager::pushSurface(MenuPivot);
}

//----------------------------------------------------------------

void
MenuBetting::doOddsBets()
{
    // TODO
}

//----------------------------------------------------------------

void
MenuBetting::doBetFlags()
{
    // TODO
}

//----------------------------------------------------------------

void
MenuBetting::doRemoveBets()
{
    // TODO
}

//----------------------------------------------------------------

void
MenuBetting::doUndoLastBet()
{
    // TODO
}

//----------------------------------------------------------------

void
MenuBetting::doQuickBet()
{
    // TODO
}

//----------------------------------------------------------------

void
MenuBetting::doRollDice()
{
    // TODO
}

//----------------------------------------------------------------

void
MenuBetting::back()
{
    // Set our own state in base class to reflect cancel.
    // Also informs parent surfaces of the state of operation.
    // In turn, parent menus can decide if they are skipped
    // when unwinding the menu stack. But we're the topmost menu.

    // No need to setOperationResult, we have no parent.
    // setOperationResult(OperationResult::Cancel);  // base class

    // We're the topmost menu, nothing to pop
    // ConsoleManager::instance().popSurfaces();

    // But we do want to hide the MenuBetting if it's visible.

    // TODO
}

//----------------------------------------------------------------
