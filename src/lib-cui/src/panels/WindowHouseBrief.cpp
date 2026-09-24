//----------------------------------------------------------------
//
// File: WindowHouseBrief.cpp
//
//----------------------------------------------------------------

#include <cui/panels/WindowHouseBrief.h>
#include <cui/layouts/LayoutCrapsScreen.h>
#include <cui/CuiUtils.h>
#include <cui/CuiStructs.h>
#include <gen/Logger.h>
#include <iomanip>
#include <sstream>
#include <string>

using namespace Cui;

//----------------------------------------------------------------

WindowHouseBrief::WindowHouseBrief()
    : PanelBase("WindowHouseBrief")
{
    newWindow(Layout::houseBriefHeight,           // In base class
              Layout::houseBriefWidth,
              Layout::houseBriefTopRow,
              Layout::houseBriefLeftCol);
}

//----------------------------------------------------------------

WindowHouseBrief&
WindowHouseBrief::instance()
{
    static WindowHouseBrief hb;
    return hb;
}

//----------------------------------------------------------------
//
//  ┬─────────────────────────────┐
//  │ House Bal: +$2,050          │
//  │ NumBets: 128 (68W, 60L)     │
//  │ Pct: 53.12% win 46.87% lose │
//  │ Last: -$100 300W,200L       │
//  │ On table: 12 bets, $1,520   │
//  ├─────────────────────────────┤
//
void
WindowHouseBrief::draw()
{
    werase(pWin_);

    drawExternalJunctions();
    drawInternalBorders();
    drawStaticContent();
    populate();

    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------
//
// We need to touch up border junctions to mate with our internal
// lines. But the border is outside of our window. Ask LayoutCrapsTable
// to take of it.
//
void
WindowHouseBrief::drawExternalJunctions()
{
    // Have none
}

//----------------------------------------------------------------

void
WindowHouseBrief::drawInternalBorders()
{
    // Have none
}

//----------------------------------------------------------------
//
// Draw static field contents
//
void
WindowHouseBrief::drawStaticContent()
{
    // Have none
}

//----------------------------------------------------------------
//
// Updates all dynamic field content.
//
void
WindowHouseBrief::populate()
{
    populateNetBalance();
    populateNumBets();
    populatePct();
    populateOnTable();
    populateLast();
}

//----------------------------------------------------------------

void
WindowHouseBrief::populateNetBalance()
{
    std::string plusOrMinus("+");
    if (netBalance_ < 0) plusOrMinus = "-";

    std::string s = "House Net: " + plusOrMinus +
        Gen::MoneyUtils::toString(static_cast<unsigned>(netBalance_));

    mvwaddstr(pWin_, 0, 1, s.c_str());
}

//----------------------------------------------------------------

void
WindowHouseBrief::populateNumBets()
{
    std::string s = "NumBets: ";
    s += std::to_string(numBetsTableWins_ + numBetsTableLoses_) + " ";
    s += std::to_string(numBetsTableWins_)  + "W,";
    s += std::to_string(numBetsTableLoses_) + "L";

    mvwaddstr(pWin_, 1, 1, s.c_str());
}

//----------------------------------------------------------------

void
WindowHouseBrief::populatePct()
{
    std::string s = tableWinPercentages();
    mvwaddstr(pWin_, 2, 1, s.c_str());
}

//----------------------------------------------------------------

std::string
WindowHouseBrief::tableWinPercentages() const
{
    const unsigned total = numBetsTableWins_ + numBetsTableLoses_;

    if (total == 0)
    {
        return "Pct: 0.00% win 0.00% lose";
    }

    const double winPercent =
        100.0 * static_cast<double>(numBetsTableWins_) / total;

    const double losePercent =
        100.0 * static_cast<double>(numBetsTableLoses_) / total;

    std::ostringstream out;
    out << "Pct: "
        << std::fixed << std::setprecision(2)
        << winPercent << "% win "
        << losePercent << "% lose";

    return out.str();
}

//----------------------------------------------------------------

void
WindowHouseBrief::populateLast()
{
    std::string s = "Last: ";
    std::string plusOrMinus("+");

    int total = tableIntakeLastRoll_ - tableOutputLastRoll_;

    if (total < 0)
    {
        plusOrMinus = "-";
    }

    s += plusOrMinus + "$" + std::to_string(total) + " ";
    s += std::to_string(tableIntakeLastRoll_) + "W,";
    s += std::to_string(tableOutputLastRoll_) + "L";

    mvwaddstr(pWin_, 3, 1, s.c_str());
}

//----------------------------------------------------------------

void
WindowHouseBrief::populateOnTable()
{
    std::string s = "On table: ";
    s += std::to_string(numBetsOnTable_) + " bets, ";
    s += Gen::MoneyUtils::toString(amtOnTable_);

    mvwaddstr(pWin_, 4, 1, s.c_str());
}

//----------------------------------------------------------------

void
WindowHouseBrief::onTableResults(
    Gen::Money balance,              // how much left in bank
    int netBalance,                  // profit/loss session start
    unsigned numBetsTableWins,       // table wins session start, players lose
    unsigned numBetsTableLoses,      // table lose session start, players win
    Gen::Money tableIntakeLastRoll,  // table won last roll
    Gen::Money tableOutputLastRoll)  // table lost last roll
{
    balance_             = balance;
    netBalance_          = netBalance;
    numBetsTableWins_    = numBetsTableWins;
    numBetsTableLoses_   = numBetsTableLoses;
    tableIntakeLastRoll_ = tableIntakeLastRoll;
    tableOutputLastRoll_ = tableOutputLastRoll;
}

//----------------------------------------------------------------

void
WindowHouseBrief::onTableNumBetsOnTableChanged(
    unsigned numBetsOnTable, Gen::Money amtOnTable)
{
    numBetsOnTable_ = numBetsOnTable;
    amtOnTable_ = amtOnTable;
}

//----------------------------------------------------------------
