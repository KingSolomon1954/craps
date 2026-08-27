//----------------------------------------------------------------
//
// File: MenuOddsBet.cpp
//
//----------------------------------------------------------------

#include <cui/MenuOddsBet.h>
#include <controller/CrapsReaders.h>
#include <cui/ConsoleManager.h>
#include <gen/MoneyUtils.h>
#include <gen/ErrorPass.h>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <ncurses.h>

using namespace Cui;

//----------------------------------------------------------------

MenuOddsBet::MenuOddsBet()
{
    if (pWin_ != nullptr) delwin(pWin_);
}

//----------------------------------------------------------------

MenuOddsBet&
MenuOddsBet::instance()
{
    static MenuOddsBet menu;
    return menu;
}

//----------------------------------------------------------------

MenuOddsBet::~MenuOddsBet()
{
}

//----------------------------------------------------------------
//
// Draw the menu
//
// Overrides menu base class.
//
// Dynamically builds the menu prompts from list of player's odds bets.
// Shows only active odds capable bets (sorted most recent first).
// 
// Looks something like this:
//     
// 0   ┌─────────────────────────────────┐
// 1   │ Which Odds Bet                  │
// 2   ├─────────────────────────────────┤
// 3   │ [1] Pass Line 6 ($200, $0)      │
// 4   │ [2] Come Bet 8 ($1,000, $3,000) │
// 5   │ [3] Don't Pass 10 ($500, $500)  │
// 6   │ [esc] Back                      │
// 7   └─────────────────────────────────┘
// 
void
MenuOddsBet::draw()
{
    gatherBets();
    buildMenuEntries();

    windowResize();
        
    werase(pWin_);
    drawBorders();
    drawStaticContent();
    populate();
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------
//
// Create a vector of bets that are odds capable.
//
void
MenuOddsBet::gatherBets()
{
    getPlayerId();     // playerId_ is set
    gatherBetsInfo();  // bets_ is populated
}

//----------------------------------------------------------------

void
MenuOddsBet::getPlayerId()
{
    Gen::ErrorPass  ep;
    auto rc = Ctrl::CrapsReaders::getUserPlayer(playerId_, ep);
    assert(rc == Gen::ReturnCode::Success);
}

//----------------------------------------------------------------

void
MenuOddsBet::gatherBetsInfo()
{
    BetIdList betIdList;
    getBetIdList(betIdList);   // fills in betIdList
    gatherBetsDetail(betList); // uses betIdList
}

//----------------------------------------------------------------

void
MenuOddsBet::getBetIdList(BetIdList& betIdList)
{
    rc = Ctrl::CrapsReaders::readPlayerGetOddsBets(playerId_, betIdList, ep); 
    assert(rc == Gen::ReturnCode::Success);
}

//----------------------------------------------------------------

void
MenuOddsBet::gatherBetsDetail(const BetIdList& betIdList)
{
    // TODO CrapsReaders needs to return a struct of Bet details
    Gen::ErrorPass ep;
    
    bets_.clear();
    for (auto id : betIdList)
    {
        BetEntry be;

        be.betId = id;
        Ctrl::CrapsReaders::readBetPlayerId      (id, be.playerId,          ep);
        Ctrl::CrapsReaders::readBetName          (id, be.betName,           ep);
        Ctrl::CrapsReaders::readBetPivot         (id, be.pivot,             ep);
        Ctrl::CrapsReaders::readBetContractAmount(id, be.contractAmount,    ep);
        Ctrl::CrapsReaders::readBetOddsAmount    (id, be.currentOddsAmount, ep);
        Ctrl::CrapsReaders::readBetWhenCreated   (id, be.whenCreated,       ep);
        
        bets_.push_back(be);
    }
    sortBetsByCreated();
}

//----------------------------------------------------------------

void
MenuOddsBet::sortBetsByCreated()
{
    std::sort(bets_.begin(), bets_.end(),
        [](const BetEntry& a, const BetEntry& b)
        {
            return a.whenCreated > b.whenCreated;  // newest first
        });
}

//----------------------------------------------------------------

void
MenuOddsBet::buildMenuEntries()
{
    menuEntries_.clear();
    buildMenuBets();
    addQuitEntry();
}

//----------------------------------------------------------------

void
MenuOddsBet::buildMenuBets()
{
    for (size_t i = 0; i < activeOddsBets_.size(); ++i)
    {
        const char hotKey = indexToHotKey(i);

        menuEntries_.push_back({
            .hotKey = hotKey,
            .betId  = activeOddsBets_[i].betId,
            .text   = formatHotKey(hotKey) + " " +
                      formatBet(activeOddsBets_[i])
        });
    }
}

//----------------------------------------------------------------

char
MenuOddsBet::indexToHotKey(size_t index)
{
    if (index >= MenuHotKeys.size())
        throw std::out_of_range("Too many menu entries");

    return MenuHotKeys[index];
}

//----------------------------------------------------------------

void
MenuOddsBet::addQuitEntry()
{
    menuEntries_.push_back({.hotKey = 27, .betId = 0, .text = "[esc] Back"});
}

//----------------------------------------------------------------

std::string
MenuOddsBet::formatBet(const BetInfo& bet)
{
    std::string name  = EnumBetName::toString(bet.betName);
    std::string amt    = formatAmount(bet);
    std::string pivot  = formatPivot(bet);
    return name + " " + pivot + " " + amt;
}

//----------------------------------------------------------------

std::string
MenuOddsBet::formatAmount(const BetInfo& bet)
{
    std::string s = "($" + std::to_string(bet.contractAmount);
    s += ", $" + std::to_string(bet.currentOddsAmount) + ")";
    return s;
}

//----------------------------------------------------------------

std::string
MenuOddsBet::formatPivot(const BetInfo& bet)
{
    return std::to_string(bet.pivot);
}

//----------------------------------------------------------------

std::string
MenuOddsBet::formatHotKey(const char hotKey)
{
    return "[" + std::string(1, hotKey) + "]";
}

//----------------------------------------------------------------

void
MenuOddsBet::windowResize()
{
    const auto [height, width] = calcSize();

    if (pWin_ == nullptr)
    {
        pWin_ = newwin(height, width, winBorderTopCol_, winBorderTopRow_);
        if (pWin_ == nullptr)
        {
            throw std::runtime_error("Unable to create menu window");
        }
    }
    else
    {
        wresize(pWin_, height, width);
        mvwin(pWin_, winBorderTopCol_, winBorderTopRow_);
    }
}

//----------------------------------------------------------------

std::pair<int, int>
MenuOddsBet::calcSize() const
{
    constexpr int rowsAbove  = 3; // Top border + title area
    constexpr int rowsBelow  = 1; // bottom border
    constexpr int borderCols = 2; // 2 vertical borders
    constexpr int blankCols  = 2; // 2 empty columns adjacent ot borders

    int height = menuEntries_.size() + rowsAbove + rowsBelow;

    int width = 0;
    for (const auto& e : menuEntries_)
    {
        width = std::max(width, static_cast<int>(e.text.size()));
    }
    width += blankCols + borderCols;
    return {height, width};
}

//----------------------------------------------------------------

void
MenuOddsBet::drawBorders()
{
    box(pWin_, 0, 0);

    int height, width;
    getmaxyx(pWin_, height, width);

    // Horizontal separator below the title.
    mvwhline(pWin_, 2, 1, ACS_HLINE, width - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, width - 1, ACS_RTEE);
}

//----------------------------------------------------------------

void
MenuOddsBet::drawStaticContent()
{
    // Menu title bar
   mvwprintw(pWin_, 1, 2, "Which Odds Bet");
}

//----------------------------------------------------------------

void
MenuOddsBet::populate()
{
    for (size_t i = 0; i < menuEntries_.size(); ++i)
    {
        mvwaddstr(pWin_, i + 3, 2, menuEntries_[i].text.c_str());
    }
}

//----------------------------------------------------------------
//
// Override menu base class
//
void
MenuOddsBet::handleMenuKey(int ch)
{
    if (ch == 27)  // escape
    {
        quit();
        return;
    }

    auto it = std::find_if(
        menuEntries_.begin(), menuEntries_.end(),
        [ch](const MenuEntry& entry)
        {
            return entry.hotKey == ch;
        });

    if (it == menuEntries_.end()) return;

    processSelection(it->betId);
}

//----------------------------------------------------------------

void
MenuOddsBet::quit()
{
    // Set our own state in base class to reflect cancel.
    // Also informs parent surfaces of the state of operation.
    // In turn, parent menus can decide if they are skipped
    // when unwinding the menu stack.
    //
    setOperationResult(OperationResult::cancel);  // base class
    ConsoleManager::popSurfaces();
}

//----------------------------------------------------------------

void
MenuOddsBet::processSelection(const Craps::BetId& betId)
{
    const BetInfo& bet = findBet(betId);
    populateCarrier(bet);
    prepDialogAmount(bet);
    activateDialogAmount();
}

//----------------------------------------------------------------

BetInfo&
MenuOddsBet::findBet(const Craps::BetId& betId)
{
    auto it = std::find_if(
        activeOddsBets_.begin(),
        activeOddsBets_.end(),
        [betId](const BetInfo& bet)
        {
            return bet.betId == betId;
        });

    if (it == activeOddsBets_.end())
    {
        throw std::runtime_error(
            "Unable to match betId. Programmer error.");
    }
}

//----------------------------------------------------------------

void
MenuOddsBet::populateCarrier(const BetInfo& bet)
{
    CarrierBet::clear();
    CarrierBet::setBetType(bet.betName);
    CarrierBet::setPivot(bet.pivot);
}

//----------------------------------------------------------------

void
MenuOddsBet::prepDialogAmount(const BetInfo& bet)
{
    DialogBetAmount::setPrompt("Odds on %s", bet.pivot);
    auto amount = getAutoFillAmount(OddsBet, bet.pivot);
    DialogBetAmount::preFill(amount);
}

//----------------------------------------------------------------

void
MenuOddsBet::activateDialogAmount()
{
    ConsoleManager::pushSurface(AmountDialog);
}

//----------------------------------------------------------------
