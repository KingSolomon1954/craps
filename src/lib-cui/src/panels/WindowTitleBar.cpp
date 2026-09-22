//----------------------------------------------------------------
//
// File: WindowTitleBar.cpp
//
//----------------------------------------------------------------

#include <cui/panels/WindowTitleBar.h>
#include <cui/layouts/LayoutCrapsScreen.h>
#include <cui/layouts/LayoutTitleBar.h>
#include <cui/CuiStructs.h>
#include <cui/CuiUtils.h>
#include <controller/CrapsReaders.h>
#include <gen/Logger.h>

using namespace Cui;

//----------------------------------------------------------------

WindowTitleBar::WindowTitleBar()
    : PanelBase("WindowTitleBar")
{
    newWindow(Layout::titleBarHeight,           // In base class
              Layout::titleBarWidth,
              Layout::titleBarTopRow,
              Layout::titleBarLeftCol);
    initTableInfo();
}
    
//----------------------------------------------------------------

WindowTitleBar&
WindowTitleBar::instance()
{
    static WindowTitleBar wt;
    return wt;
}

//----------------------------------------------------------------

void
WindowTitleBar::initTableInfo()
{
    tableName_  = Ctrl::CrapsReaders::readTableName();
    auto r = Ctrl::CrapsReaders::readTableMinMaxRules();
    buildRulesField(r.minLineBet, r.maxLineBet, r.maxOdds);
}

//----------------------------------------------------------------

void
WindowTitleBar::buildRulesField(unsigned minLineBet,
                                unsigned maxLineBet,
                                unsigned maxOdds)
{
    using L = Layout;

    std::string minStr  = "Min $" + std::to_string(minLineBet);
    std::string maxStr  = "Max $" + std::to_string(maxLineBet);
    std::string oddsStr = "Odds " + std::to_string(maxOdds) + "x";
    rules_ = minStr + " " + maxStr + " " + oddsStr;
    
    // Force rules_ to fit in width of the field
    if (rules_.length() > L::f4Cols) rules_.erase(L::f4Cols);
}

//----------------------------------------------------------------

void
WindowTitleBar::draw()
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
WindowTitleBar::drawExternalJunctions()
{
    LayoutCrapsScreen::instance().drawExternalJunctionsTitleBar();
}

//----------------------------------------------------------------

void
WindowTitleBar::drawInternalBorders()
{
    using T = LayoutTitleBar;

    mvwaddch(pWin_, 0, T::col1 - 1, ACS_VLINE);
    mvwaddch(pWin_, 0, T::col2 - 1, ACS_VLINE);
    mvwaddch(pWin_, 0, T::col3 - 1, ACS_VLINE);
}

//----------------------------------------------------------------
//
// Draw static field contents
//
void
WindowTitleBar::drawStaticContent()
{
    using L = Layout;

    mvwaddnstr(pWin_, L::f1LabelRow, L::f1LabelCol, L::f1Label.data(),
               static_cast<int>(Layout::f1Label.size()));
    mvwaddnstr(pWin_, L::f2LabelRow, L::f2LabelCol, L::f2Label.data(),
               static_cast<int>(Layout::f2Label.size()));
}

//----------------------------------------------------------------
//
// Updates all dynamic field content.
//
// String will usually be a couple of chars longer than CharLimit,
// but the WINDOW will cut off the extra. This is actually
// desired, so partial next rolls are visible.
//
void
WindowTitleBar::populate()
{
    populatePoint();
    populateRollCount();
    populateTableName();
    populateTableMaxOdds();
}

//----------------------------------------------------------------

void
WindowTitleBar::populatePoint()
{
    using L = Layout;
    
    wattron(pWin_, COLOR_PAIR(ColorPairs::Point));
    mvwprintw(pWin_, L::f1Row, L::f1Col, L::f1Fmt.data(), point_);
    wattroff(pWin_, COLOR_PAIR(ColorPairs::Point));
}

//----------------------------------------------------------------

void
WindowTitleBar::populateRollCount()
{
    using L = Layout;
    mvwprintw(pWin_, L::f2Row, L::f2Col, L::f2Fmt.data(), rollCount_);    
}

//----------------------------------------------------------------

void
WindowTitleBar::populateTableName()
{
    using L = Layout;
    mvwprintw(pWin_, L::f3Row, L::f3Col, L::f3Fmt.data(), tableName_.c_str());
}

//----------------------------------------------------------------

void
WindowTitleBar::populateTableMaxOdds()
{
    using L = Layout;
    
    // Center string within field width
    int startCol = L::f4Col + ((L::f4Cols - rules_.length()) / 2);
    
    mvwprintw(pWin_, L::f4Row, startCol, "%s", rules_.c_str());
}

//----------------------------------------------------------------

void
WindowTitleBar::onDiceNewValue(unsigned d1, unsigned d2, unsigned rollCount)
{
    (void) d1; (void) d2;
    rollCount_ = rollCount;
}

//----------------------------------------------------------------

void
WindowTitleBar::onPointEstablished(unsigned point)
{
    point_ = point;
}

//----------------------------------------------------------------

void
WindowTitleBar::onSevenOut()
{
    point_ = 0;
}

//----------------------------------------------------------------

void
WindowTitleBar::onPassLineWinner()
{
    point_ = 0;
}

//----------------------------------------------------------------
