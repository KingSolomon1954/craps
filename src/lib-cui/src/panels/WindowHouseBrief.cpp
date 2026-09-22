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
//  │ On table: 12 bets, $1,520   │
//  │ Last: 300W,200L,-$100       │
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
    populateBalance();
    populateNumBets();
    populatePct();
    populateOnTable();
    populateLast();
}

//----------------------------------------------------------------

void
WindowHouseBrief::populateBalance()
{
}

//----------------------------------------------------------------

void
WindowHouseBrief::populateNumBets()
{
}

//----------------------------------------------------------------

void
WindowHouseBrief::populatePct()
{
}

//----------------------------------------------------------------

void
WindowHouseBrief::populateOnTable()
{
}

//----------------------------------------------------------------

void
WindowHouseBrief::populateLast()
{
}

//----------------------------------------------------------------
