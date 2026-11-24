//----------------------------------------------------------------
//
// File: MenuBetFlags.cpp
//
//----------------------------------------------------------------

#include <cui/menus/MenuBetFlags.h>
#include <cui/managers/OperationManager.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/core/CuiUtils.h>
#include <controller/CrapsReaders.h>
#include <gen/Logger.h>
#include <algorithm>

using namespace Cui;
using Ctrl::BetFlags::Category;
using Ctrl::BetFlags::Scope;

//----------------------------------------------------------------

MenuBetFlags::MenuBetFlags()
    : MenuBase("MenuBetFlags")
{
    // Create an initial WINDOW at location 0,0. Gets resized later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);

    // Dynamically built menu, so no initial fillWindow();
}

//----------------------------------------------------------------

MenuBetFlags&
MenuBetFlags::instance()
{
    static MenuBetFlags menu;
    return menu;
}

//----------------------------------------------------------------
//
// Only kinds of bets the player has are offered. When no bet can take a
// flag the menu is not opened at all; MenuBetting says so instead.
//
void
MenuBetFlags::buildMenuEntries()
{
    menuEntries_.clear();

    const auto flagBets = Ctrl::CrapsReaders::readFlagBets();
    auto has = [&flagBets](Category category)
    {
        return std::any_of(flagBets.begin(), flagBets.end(),
            [category](const Ctrl::CrapsReaders::FlagBet& fb)
            { return fb.category == category; });
    };

    if (!flagBets.empty())
    {
        menuEntries_.push_back({'i', Scope::Bet, "[i] Individual Bet"});
        menuEntries_.push_back({'a', Scope::All, "[a] All Bets"});
    }
    if (has(Category::Odds))
        menuEntries_.push_back({'o', Scope::Odds,    "[o] Odds Bets"});
    if (has(Category::Place))
        menuEntries_.push_back({'p', Scope::Place,   "[p] Place Bets"});
    if (has(Category::Buy))
        menuEntries_.push_back({'b', Scope::Buy,     "[b] Buy Bets"});
    if (has(Category::Lay))
        menuEntries_.push_back({'l', Scope::Lay,     "[l] Lay Bets"});
    if (has(Category::Hardway))
        menuEntries_.push_back({'h', Scope::Hardway, "[h] Hardway Bets"});

    menuEntries_.push_back({27, Scope::All, "[. or esc] Back"});
}

//----------------------------------------------------------------

void
MenuBetFlags::onAttach(SurfaceBase* pParent)
{
    SurfaceBase::onAttach(pParent);
    buildMenuEntries();
    calcSize();
}

//----------------------------------------------------------------

void
MenuBetFlags::calcSize()
{
    constexpr int rowsAbove  = 3; // Top border + title area
    constexpr int rowsBelow  = 1; // bottom border
    constexpr int borderCols = 2; // 2 vertical borders
    constexpr int blankCols  = 2; // 2 empty columns adjacent to borders

    winSize_.rows = static_cast<int>(menuEntries_.size()) + rowsAbove + rowsBelow;

    int width = static_cast<int>(std::string("Bet Flags").size());
    for (const auto& e : menuEntries_)
    {
        width = std::max(width, static_cast<int>(e.text.size()));
    }
    winSize_.cols = width + blankCols + borderCols;
}

//----------------------------------------------------------------

LocationRequest
MenuBetFlags::getLocationRequest() const
{
    LocationRequest req;
    req.kind      = LocationKind::Menu;
    req.size.rows = winSize_.rows;
    req.size.cols = winSize_.cols;
    req.direction = Direction::Right;
    return req;
}

//----------------------------------------------------------------

void
MenuBetFlags::setLocation(WindowPosition pos)
{
    winPos_ = pos;
    resize(winSize_);  // Dynamic menu needs resizing
    repos(winPos_);    // and repositioning
}

//----------------------------------------------------------------
//
// 0   ┌────────────────────┐
// 1   │ Bet Flags          │
// 2   ├────────────────────┤
// 3   │ [i] Individual Bet │
// 4   │ [a] All Bets       │
// 5   │ [o] Odds Bets      │
// 6   │ [p] Place Bets     │
// 7   │ [. or esc] Back    │
// 8   └────────────────────┘
//
void
MenuBetFlags::draw()
{
    werase(pWin_);
    drawBorders();
    mvwprintw(pWin_, 1, 2, "Bet Flags");
    populate();
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------

void
MenuBetFlags::drawBorders()
{
    box(pWin_, 0, 0);
    mvwhline(pWin_, 2, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, winSize_.cols - 1, ACS_RTEE);
}

//----------------------------------------------------------------

void
MenuBetFlags::populate()
{
    for (size_t i = 0; i < menuEntries_.size(); ++i)
    {
        mvwaddstr(pWin_, static_cast<int>(i) + 3, 2,
                  menuEntries_[i].text.c_str());
    }
}

//----------------------------------------------------------------

bool
MenuBetFlags::handleKey(int ch)
{
    if (ch == 27 || ch == '.')  // . or escape
    {
        back();
        return true;
    }

    const int key = (ch < 128) ? std::tolower(ch) : ch;
    auto it = std::find_if(
        menuEntries_.begin(), menuEntries_.end(),
        [key](const MenuEntry& entry)
        {
            return entry.hotKey == key;
        });

    if (it == menuEntries_.end()) return false;

    LOG_TRACE("MenuBetFlags::handleKey() " + it->text);
    (void) OperationManager::instance().betFlags(it->scope);
    return true;
}

//----------------------------------------------------------------

void
MenuBetFlags::back()
{
    setOperationResult(OperationResult::Cancel);  // base class
    SurfaceManager::instance().popSurfaces();
}

//----------------------------------------------------------------
