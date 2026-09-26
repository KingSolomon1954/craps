//----------------------------------------------------------------
//
// File: WindowPlayerBrief.cpp
//
//----------------------------------------------------------------

#include <cui/panels/WindowPlayerBrief.h>
#include <cui/layouts/LayoutCrapsScreen.h>
#include <cui/CuiUtils.h>
#include <cui/CuiStructs.h>
#include <controller/CrapsReaders.h>
#include <craps/LastRollStats.h>
#include <gen/ErrorPass.h>
#include <gen/Logger.h>
#include <iomanip>
#include <sstream>
#include <string>

using namespace Cui;

//----------------------------------------------------------------

WindowPlayerBrief::WindowPlayerBrief()
    : PanelBase("WindowPlayerBrief")
{
    newWindow(Layout::playerBriefHeight,           // In base class
              Layout::playerBriefWidth,
              Layout::playerBriefTopRow,
              Layout::playerBriefLeftCol);
    initPlayer();
}

//----------------------------------------------------------------

WindowPlayerBrief&
WindowPlayerBrief::instance()
{
    static WindowPlayerBrief hb;
    return hb;
}

//----------------------------------------------------------------

void
WindowPlayerBrief::initPlayer()
{
    Gen::ErrorPass ep;
    auto rc = Ctrl::CrapsReaders::getUserPlayer(playerId_, ep);
    assert(rc == Gen::ReturnCode::Success);
    
    playerName_ = Ctrl::CrapsReaders::getUserPlayerName();
}

//----------------------------------------------------------------
//
//  ┬─────────────────────────────┐
//  │ Player: Nathan (shooter)    │
//  │ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ │
//  │ Bal: $2,000 Net: +$1,044    │
//  │ NumBets: 55 (21W, 34L)      │
//  │ Pct: 38.18% win 61.81% lose │
//  │ Last: 180W,100L,+$80,       │
//  │ On table: 3 bets, $320      │
//  ├─────────────────────────────┤
//
void
WindowPlayerBrief::draw()
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
WindowPlayerBrief::drawExternalJunctions()
{
    // Have none
}

//----------------------------------------------------------------

void
WindowPlayerBrief::drawInternalBorders()
{
    std::string s(" ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~");
    mvwaddstr(pWin_, 1, 0, s.c_str());
}

//----------------------------------------------------------------
//
// Draw static field contents
//
void
WindowPlayerBrief::drawStaticContent()
{
    // Have none
}

//----------------------------------------------------------------
//
// Updates all dynamic field content.
//
void
WindowPlayerBrief::populate()
{
    populateName();
    populateBalance();
    populateNumBets();
    populatePct();
    populateOnTable();
    populateLast();
}

//----------------------------------------------------------------

void
WindowPlayerBrief::populateName()
{
    std::string n = playerName_;
    n += shooter_;
    
    mvwaddstr(pWin_, 0, 1, n.c_str());
}

//----------------------------------------------------------------

void
WindowPlayerBrief::populateBalance()
{
    //  │ Bal: $2,000 Net: +$8,280       │

    std::string plusOrMinus("+");
    if (netBalance_ < 0) plusOrMinus = "-";

    std::string s = "Bal: " + Gen::MoneyUtils::toString(balance_);
    s += " (" + plusOrMinus + Gen::MoneyUtils::toString(netBalance_);
    s += ")";

    mvwaddstr(pWin_, 2, 1, s.c_str());
}

//----------------------------------------------------------------

void
WindowPlayerBrief::populateNumBets()
{
    std::string s = "NumBets: ";
    s += std::to_string(numBetsPlayerWins_ + numBetsPlayerLoses_) + " ";
    s += std::to_string(numBetsPlayerWins_)  + "W,";
    s += std::to_string(numBetsPlayerLoses_) + "L";
    
    mvwaddstr(pWin_, 3, 1, s.c_str());
}

//----------------------------------------------------------------

void
WindowPlayerBrief::populatePct()
{
    std::string s = playerWinPercentages();
    mvwaddstr(pWin_, 4, 1, s.c_str());
}

//----------------------------------------------------------------

std::string
WindowPlayerBrief::playerWinPercentages() const
{
    const unsigned total = numBetsPlayerWins_ + numBetsPlayerLoses_;

    if (total == 0)
    {
        return "Pct: 0.00% win 0.00% lose";
    }

    const double winPercent =
        100.0 * static_cast<double>(numBetsPlayerWins_) / total;

    const double losePercent =
        100.0 * static_cast<double>(numBetsPlayerLoses_) / total;

    std::ostringstream out;
    out << "Pct: "
        << std::fixed << std::setprecision(2)
        << winPercent << "% win "
        << losePercent << "% lose";

    return out.str();
}

//----------------------------------------------------------------

void
WindowPlayerBrief::populateLast()
{
    std::string s = "Last: ";
    std::string plusOrMinus("+");
    
    int total = playerIntakeLastRoll_ - playerOutputLastRoll_;
    
    if (total < 0)
    {
        plusOrMinus = "-";
    }

    s += plusOrMinus + "$" + std::to_string(total) + " ";
    s += std::to_string(playerIntakeLastRoll_) + "W,";
    s += std::to_string(playerOutputLastRoll_) + "L";

    mvwaddstr(pWin_, 5, 1, s.c_str());
}

//----------------------------------------------------------------

void
WindowPlayerBrief::populateOnTable()
{
    std::string s = "On table: ";
    s += std::to_string(numBetsOnTable_) + " bets, ";
    s += Gen::MoneyUtils::toString(amtOnTable_);

    mvwaddstr(pWin_, 6, 1, s.c_str());
}

//----------------------------------------------------------------

void
WindowPlayerBrief::onPlayerBalanceChanged(
    Craps::PlayerId playerId,
    Gen::Money      balance,      // how much left in wallet
    int             netBalance)   // profit/loss this session
{
    if (playerId != playerId_) return;
    
    balance_    = balance;    
    netBalance_ = netBalance;
}

//----------------------------------------------------------------

void
WindowPlayerBrief::onResolveBetsEnd()
{
    Gen::ErrorPass ep;
    Craps::LastRollStats lrs;
    
    auto rc = Ctrl::CrapsReaders::readPlayerLastRollStats(playerId_, lrs, ep);
    assert(rc == Gen::ReturnCode::Success);

    numBetsPlayerWins_    = lrs.numBetsWin;
    numBetsPlayerLoses_   = lrs.numBetsLose;
    playerIntakeLastRoll_ = lrs.amountWin;
    playerOutputLastRoll_ = lrs.amountLose;
    numBetsOnTable_       = lrs.numBetsOnTable;
    amtOnTable_           = lrs.amountOnTable;
}

//----------------------------------------------------------------

void
WindowPlayerBrief::onNewShooter(const Craps::PlayerId& playerId)
{
    if (playerId != playerId_)
    {
        shooter_ = " (shooter)";
    }
    else
    {
        shooter_.clear();
    }
}

//----------------------------------------------------------------

