//----------------------------------------------------------------
//
// File: MenuOddsBet.cpp
//
//----------------------------------------------------------------

#include <cui/menus/MenuOddsBet.h>
#include <cui/managers/CarrierBet.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/dialogs/DialogBetAmount.h>
#include <cui/core/CuiUtils.h>
#include <gen/MoneyUtils.h>
#include <gen/Debug.h>
#include <gen/ErrorPass.h>
#include <gen/Logger.h>
#include <algorithm>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

MenuOddsBet::MenuOddsBet()
    : MenuBase("MenuOddsBet")
{
    // Create an initial WINDOW at location 0,0. Gets resized later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
    
    // Dynamically built menu, so no initial fillWindow();
}

//----------------------------------------------------------------

MenuOddsBet&
MenuOddsBet::instance()
{
    static MenuOddsBet menu;
    return menu;
}

//----------------------------------------------------------------

void
MenuOddsBet::gatherBets()
{
    Craps::BetIds betIds;
    getBetIds(betIds);        // fills in betIdList
    gatherBetsDetail(betIds); // bets_ is now populated
    
    LOG_TRACE("Exiting MenuOddsBet::gatherBets()");
}

//----------------------------------------------------------------

void
MenuOddsBet::getBetIds(Craps::BetIds& betIds)
{
    Gen::ErrorPass ep;
    auto pid = CarrierBet::instance().getPlayerId();
    auto rc = Ctrl::CrapsReaders::readPlayerGetOddsBets(pid, betIds, ep); 
    assert(rc == Gen::ReturnCode::Success);
}

//----------------------------------------------------------------

void
MenuOddsBet::gatherBetsDetail(const Craps::BetIds& ids)
{
    Gen::ErrorPass ep;
    
    activeOddsBets_.clear();
    for (auto id : ids)
    {
        BetInfo bi;

        bi.betId = id;
        LOG_DEBUG("MenuOddsBet::gatherBetsDetail() betId:" + std::to_string(id));
        Ctrl::CrapsReaders::readBetPlayerId      (id, bi.playerId,          ep);
        Ctrl::CrapsReaders::readBetName          (id, bi.betName,           ep);
        Ctrl::CrapsReaders::readBetPivot         (id, bi.pivot,             ep);
        Ctrl::CrapsReaders::readBetContractAmount(id, bi.contractAmount,    ep);
        Ctrl::CrapsReaders::readBetOddsAmount    (id, bi.currentOddsAmount, ep);
        Ctrl::CrapsReaders::readBetWhenCreated   (id, bi.whenCreated,       ep);
        
        activeOddsBets_.push_back(bi);
        D(logBetInfo(bi);)
    }
    sortBetsByCreated();
}

//----------------------------------------------------------------

void
MenuOddsBet::logBetInfo(const BetInfo& bi)
{
    std::string s = "\nMenuOddsBet BetInfo: "
        "    \n      playerId: " + bi.playerId +
        "    \n         betId: " + std::to_string(bi.betId)          +
        "    \n       betName: " + EnumBetName::toString(bi.betName) +
        "    \n      betPivot: " + std::to_string(bi.pivot)          +
        "    \ncontractAmount: " + std::to_string(bi.contractAmount) +
        "    \n    oddsAmount: " + std::to_string(bi.currentOddsAmount);
    LOG_DEBUG(s);
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
    LOG_TRACE("Exiting MenuOddsBet::buildMenuEntries()");
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
MenuOddsBet::onAttach(SurfaceBase* pParent)
{
    SurfaceBase::onAttach(pParent);
    gatherBets();
    buildMenuEntries();
    calcSize();           // Determine our height/width
                          // winSize_ is now populated
    // Next, surface manager will ask for our locationRequest
}

//----------------------------------------------------------------

void
MenuOddsBet::calcSize()
{
    constexpr int rowsAbove  = 3; // Top border + title area
    constexpr int rowsBelow  = 1; // bottom border
    constexpr int borderCols = 2; // 2 vertical borders
    constexpr int blankCols  = 2; // 2 empty columns adjacent to borders

    winSize_.rows = menuEntries_.size() + rowsAbove + rowsBelow;

    int width = 0;
    for (const auto& e : menuEntries_)
    {
        width = std::max(width, static_cast<int>(e.text.size()));
    }
    winSize_.cols = width + blankCols + borderCols;
}

//----------------------------------------------------------------
//
// SurfaceManager wants our window size and more.
// This occurs in context of SurfaceManager::pushSurface()
// SurfaceManager informs us shortly of our screen position.
// See setLocation() below. 
//
LocationRequest
MenuOddsBet::getLocationRequest() const
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
MenuOddsBet::setLocation(WindowPosition pos)
{
    winPos_ = pos;
    resize(winSize_);  // Dynamic menu needs resizing
    repos(winPos_);    // and repositioning
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
// 6   │ [. or esc] Back                 │
// 7   └─────────────────────────────────┘
// 
void
MenuOddsBet::draw()
{
    if (shortCircuit())
    {
        return;   // Only one odds bet? then skip this menu
    }
    
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

    if (activeOddsBets_.size() == 1)
    {
        shouldSkip_ = true;
        processSelection(activeOddsBets_[0].betId);
        return true;
    }
    return false;
}

//----------------------------------------------------------------

void
MenuOddsBet::drawBorders()
{
    box(pWin_, 0, 0);

    // Horizontal separator below the title.
    mvwhline(pWin_, 2, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, winSize_.cols - 1, ACS_RTEE);
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
bool
MenuOddsBet::handleKey(int ch)
{
    if (ch == 27 || ch == '.')  // . or escape
    {
        back();
        return true;
    }

    auto it = std::find_if(
        menuEntries_.begin(), menuEntries_.end(),
        [ch](const MenuEntry& entry)
        {
            return entry.hotKey == ch;
        });

    if (it == menuEntries_.end()) return false;

    processSelection(it->betId);
    return true;
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
    SurfaceManager::instance().popSurfaces();
}

//----------------------------------------------------------------

void
MenuOddsBet::processSelection(const Craps::BetId& betId)
{
    const BetInfo& bet = findBet(betId);
    populateCarrier(bet);
    activateDialogBetAmount();
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
    auto& cb = CarrierBet::instance();
    cb.setBetName(bet.betName);
    cb.setBetId(bet.betId);
    cb.setPivot(bet.pivot);
    cb.setDesireOdds();

    LOG_DEBUG("\nMenuOddsBet::populateCarrier() "
              "\n      betName:" + EnumBetName::toString(cb.getBetName()) +
              "\n        betId:" + std::to_string(cb.getBetId())          +
              "\n        pivot:" + std::to_string(cb.getPivot())          +
              "\n    isOddsBet:" + (cb.isOddsBet() ? "true" : "false" ));
}

//----------------------------------------------------------------

void
MenuOddsBet::activateDialogBetAmount()
{
    SurfaceManager::instance().pushSurface(&DialogBetAmount::instance());
}

//----------------------------------------------------------------
