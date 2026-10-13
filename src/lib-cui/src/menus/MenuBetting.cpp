//----------------------------------------------------------------
//
// File: MenuBetting.cpp
//
//----------------------------------------------------------------

#include <cui/menus/MenuBetting.h>
#include <cui/menus/MenuPivot.h>
#include <cui/menus/MenuOddsBet.h>
#include <cui/dialogs/DialogMessage.h>
#include <cui/managers/CarrierBet.h>
#include <cui/managers/SurfaceManager.h>
#include <controller/CrapsReaders.h>
#include <cui/core/CuiSender.h>
#include <cui/core/CuiUtils.h>
#include <cui/panels/WindowNavBar.h>
#include <gen/Logger.h>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

MenuBetting::MenuBetting()
    : MenuBase("MenuBetting")
{
    LOG_TRACE("Entered MenuBetting::constructor() ");
    // Create an initial WINDOW at location 0,0. Gets positioned later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
    fillWindow();
    LOG_TRACE("Leaving MenuBetting::constructor() ");
}

//----------------------------------------------------------------

MenuBetting&
MenuBetting::instance()
{
    static MenuBetting menu;
    return menu;
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
// 11 │ [b] Buy Bet           │
// 12 │ [y] Lay Bet           │
// 13 │ [e] C&E Bet           │
// 14 │ [a] Any Craps         │
// 15 │ [7] Any 7             │
// 16 │ [n] Horn Bet          │
// 17 │ [w] World Bet         │
// 18 │ [g] Bet Flags         │
// 19 │ [q] Quick Bet         │
// 20 │ [x] Remove Bets       │
// 21 │ [u] Undo Last         │
// 22 │ [r] Roll Dice         │
// 23 │ [. or esc] Back       │
// 24 └───────────────────────┘
//
void
MenuBetting::fillWindow()
{
    box(pWin_, 0, 0);

    // Horizontal separator below the title
    mvwhline(pWin_, 2, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, winSize_.cols - 1, ACS_RTEE);

    mvwaddstr(pWin_, 1, 2, "Betting Menu");

    mvwaddstr(pWin_, 3,  2, "[p] Pass Line");
    mvwaddstr(pWin_, 4,  2, "[c] Come Bet");
    mvwaddstr(pWin_, 5,  2, "[d] Dont Pass");
    mvwaddstr(pWin_, 6,  2, "[k] Dont Come");
    mvwaddstr(pWin_, 7,  2, "[o] Odds Bet");
    mvwaddstr(pWin_, 8,  2, "[l] Place Bet");
    mvwaddstr(pWin_, 9,  2, "[h] Hardways");
    mvwaddstr(pWin_, 10, 2, "[f] Field Bet");
    mvwaddstr(pWin_, 11, 2, "[i] Buy Bet");
    mvwaddstr(pWin_, 12, 2, "[y] Lay Bet");
    mvwaddstr(pWin_, 13, 2, "[e] C&E Bet");
    mvwaddstr(pWin_, 14, 2, "[a] Any Craps");
    mvwaddstr(pWin_, 15, 2, "[7] Any 7");
    mvwaddstr(pWin_, 16, 2, "[n] Horn Bet");
    mvwaddstr(pWin_, 17, 2, "[w] World Bet (todo)");
    mvwaddstr(pWin_, 18, 2, "[g] Bet Flags");
    mvwaddstr(pWin_, 19, 2, "[q] Quick Bet");
    mvwaddstr(pWin_, 20, 2, "[x] Remove Bets");
    mvwaddstr(pWin_, 21, 2, "[u] Undo Last");
    mvwaddstr(pWin_, 22, 2, "[r] Roll Dice");
    mvwaddstr(pWin_, 23, 2, "[. or esc] Back");
}

//----------------------------------------------------------------
//
// SurfaceManager wants our window size and more.
// This occurs in context of SurfaceManager::pushSurface()
// SurfaceManager informs us shortly of our screen position.
// See setLocation() below. 
//
LocationRequest
MenuBetting::getLocationRequest() const
{
    LocationRequest req;
    req.kind      = LocationKind::Menu;
    req.size.rows = winSize_.rows;
    req.size.cols = winSize_.cols;
    req.direction = Direction::Right;
    return req;
}

//----------------------------------------------------------------
//
// SurfaceManager tells us our location.
// This occurs in context of SurfaceManager::pushSurface().
// We now have enough information to create/resize our
// ncurses WINDOW. Following this, SurfaceManager will
// call draw() on us.
//
void
MenuBetting::setLocation(WindowPosition pos)
{
    winPos_ = pos;   // MenuBetting is fixed size
    repos(winPos_);  // Just need position
}

//----------------------------------------------------------------

void
MenuBetting::draw()
{
    LOG_TRACE("MenuBetting::draw() ");
    touchwin(pWin_);
    CuiUtils::transfer(pWin_); // Reuse already filled window over and over
}

//----------------------------------------------------------------
//
// Override menu base class
//
bool
MenuBetting::handleKey(int ch)
{
    betName_ = BetName::Invalid;
    bool handled = true;

    LOG_TRACE("MenuBetting::handleKey() key=" + std::to_string(ch));
    
    switch (ch)
    {
    case 'p': doBets(BetName::PassLine); break;
    case 'c': doBets(BetName::Come);     break;
    case 'd': doBets(BetName::DontPass); break;
    case 'k': doBets(BetName::DontCome); break;
    case 'l': doBets(BetName::Place);    break;
    case 'h': doBets(BetName::Hardway);  break;
    case 'f': doBets(BetName::Field);    break;
    case 'i': doBets(BetName::Buy);      break;
    case 'y': doBets(BetName::Lay);      break;
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
    default : handled = false;           break;
    }
    return handled;
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
    case BetName::Buy:
    case BetName::Lay:
    case BetName::Hardway:   doGetPivot();  break;
    default:
        assert(true);
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
    LOG_TRACE("MenuBetting::doGetAmount() " + EnumBetName::toString(betName_));
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
MenuBetting::doOddsBets()
{
    activateMenuOddsBet();
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
    LOG_TRACE("MenuBetting::doRollDice()");
    if (SurfaceManager::instance().isActiveSurface(this))
        SurfaceManager::instance().popSurface();
    CuiSender::instance().reqRollDice();
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
    MenuPivot::instance().configure(betName_);
    SurfaceManager::instance().pushSurface(&MenuPivot::instance());
}

//----------------------------------------------------------------

void
MenuBetting::activateMenuOddsBet()
{
    if (hasQualifyingOddsBets())
    {
        SurfaceManager::instance().pushSurface(&MenuOddsBet::instance());
    }
    else
    {
        DialogMessage::MessageText m;
        m.type = DialogMessage::MessageType::Info;
        m.brief = "No qualifying odds capable bets";
        m.detail = "Only Pass, DontPass, Come and "
                   "DontCome bets can have odds";
        DialogMessage::instance().configure(m);
        
        SurfaceManager::instance().pushSurface(&DialogMessage::instance());
    }
}

//----------------------------------------------------------------

bool
MenuBetting::hasQualifyingOddsBets()
{
    Gen::ErrorPass  ep;
    Craps::PlayerId pid;

    auto rc = Ctrl::CrapsReaders::getUserPlayer(pid, ep);
    assert(rc == Gen::ReturnCode::Success);

    bool haveBets;
    rc = Ctrl::CrapsReaders::readPlayerHaveAnyOddsCapableBet(
        pid, haveBets, ep); 
    assert(rc == Gen::ReturnCode::Success);
    return haveBets;
}

//----------------------------------------------------------------
//
// MenuBetting::back() has unique behavior due to being
// invisible or not.
// 
// When MenuBetting is the active surface:
//     back() means make the menu invisible
//     MenuBetting is popped off the surface stack
//     Thereby MenuBetting is invisible to the user
//     ScreenCrapsTable keyHandler() takes over
//     ScreenCrapsTable keys are passed to MenuBetting anyway
//     No need to setOperationResult(), as there is no unwinding
//     No need to set shouldSkip(), as there is no undwinding
//    
// When MenuBetting is not the active surface:
//     back() takes no action
//     ScreenCrapsTable is already the visible surface
//     No need to setOperationResult() as there is no unwinding
//     No need to set shouldSkip() as there is no unwinding
//    
void
MenuBetting::back()
{
    if (SurfaceManager::instance().isActiveSurface(this))
    {
        SurfaceManager::instance().popSurface();  // Become invisible
    }

    // Else ignore key, nothing to do. ScreenCrapsTable is visible
}

//----------------------------------------------------------------
