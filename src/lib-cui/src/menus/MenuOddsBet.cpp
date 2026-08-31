//----------------------------------------------------------------
//
// File: MenuOddsBet.cpp
//
//----------------------------------------------------------------

#include <cui/menus/MenuOddsBet.h>
#include <controller/CrapsReaders.h>
#include <cui/CarrierBet.h>
#include <cui/ConsoleManager.h>
#include <cui/CuiUtils.h>
#include <gen/MoneyUtils.h>
#include <gen/ErrorPass.h>
#include <algorithm>
#include <cassert>
#include <ncurses.h>

using namespace Cui;

//----------------------------------------------------------------

MenuOddsBet&
MenuOddsBet::instance()
{
    static MenuOddsBet menu;
    return menu;
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
    
    if (shortCircuit())
    {
        return;   // Only one odds bet? then skip this menu
    }
    
    windowResize();
    werase(pWin_);
    drawBorders();
    drawStaticContent();
    populate();
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------

bool
MenuOddsBet::shortCircuit()
{
    shouldSkip_ = false;
    if (menuEntries_.size() == 2)
    {
        shouldSkip_ = true;
        processSelection(menuEntries_[1].betId);
        return true;
    }
    return false;
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
    getBetIdList(betIdList);     // fills in betIdList
    gatherBetsDetail(betIdList); // use betIdList
}

//----------------------------------------------------------------

void
MenuOddsBet::getBetIdList(BetIdList& betIdList)
{
    Gen::ErrorPass ep;
    auto rc = Ctrl::CrapsReaders::readPlayerGetOddsBets(playerId_, betIdList, ep); 
    assert(rc == Gen::ReturnCode::Success);
}

//----------------------------------------------------------------

void
MenuOddsBet::gatherBetsDetail(const BetIdList& betIdList)
{
    // TODO CrapsReaders needs to return a struct of Bet details
    Gen::ErrorPass ep;
    
    activeOddsBets_.clear();
    for (auto id : betIdList)
    {
        BetInfo bi;

        bi.betId = id;
        Ctrl::CrapsReaders::readBetPlayerId      (id, bi.playerId,          ep);
        Ctrl::CrapsReaders::readBetName          (id, bi.betName,           ep);
        Ctrl::CrapsReaders::readBetPivot         (id, bi.pivot,             ep);
        Ctrl::CrapsReaders::readBetContractAmount(id, bi.contractAmount,    ep);
        Ctrl::CrapsReaders::readBetOddsAmount    (id, bi.currentOddsAmount, ep);
        Ctrl::CrapsReaders::readBetWhenCreated   (id, bi.whenCreated,       ep);
        
        activeOddsBets_.push_back(bi);
    }
    sortBetsByCreated();
}

//----------------------------------------------------------------

void
MenuOddsBet::sortBetsByCreated()
{
    std::sort(activeOddsBets_.begin(), activeOddsBets_.end(),
        [](const BetInfo& a, const BetInfo& b)
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
    addBackEntry();  // Add the entry to cancel
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
        throw std::out_of_range("MenuOddsBet::indexToHotKey() "
                                "Too many menu entries");
    return MenuHotKeys[index];
}

//----------------------------------------------------------------

void
MenuOddsBet::addBackEntry()
{
    menuEntries_.push_back({.hotKey = 27, .betId = 0, .text = "[. or esc] Back"});
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
        newWindow(height, width, winBorderTopCol_,    // In base class
                  winBorderTopRow_, "MenuOddsBet");
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
MenuOddsBet::handleKey(int ch)
{
    if (ch == 27)  // escape
    {
        back();
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
MenuOddsBet::back()
{
    // Set our own state in base class to reflect cancel.
    // Also informs parent surfaces of the state of operation.
    // In turn, parent menus can decide if they are skipped
    // when unwinding the menu stack.
    //
    setOperationResult(OperationResult::Cancel);  // base class
    ConsoleManager::instance().popSurfaces();
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

MenuOddsBet::BetInfo&
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
        throw std::runtime_error("MenuOddsBet::findBet() Unable "
            "to match betId. Programmer error.");
    }
    return *it;
}

//----------------------------------------------------------------

void
MenuOddsBet::populateCarrier(const BetInfo& bet)
{
    auto cb = CarrierBet::instance();
    cb.clear();
    cb.setBetType(bet.betName);
    cb.setPivot(bet.pivot);
}

//----------------------------------------------------------------

void
MenuOddsBet::prepDialogAmount(const BetInfo& bet)
{
    // TODO
    // DialogBetAmount::setPrompt("Odds on %s", bet.pivot);
    // auto amount = getAutoFillAmount(OddsBet, bet.pivot);
    // DialogBetAmount::preFill(amount);
}

//----------------------------------------------------------------

void
MenuOddsBet::activateDialogAmount()
{
    // TODO
    // ConsoleManager::pushSurface(AmountDialog::instance());
}

//----------------------------------------------------------------
