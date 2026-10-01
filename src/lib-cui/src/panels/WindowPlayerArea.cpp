//----------------------------------------------------------------
//
// File: WindowPlayerArea.cpp
//
//----------------------------------------------------------------

#include <cui/panels/WindowPlayerArea.h>
#include <cui/layouts/LayoutCrapsScreen.h>
#include <cui/CuiUtils.h>
#include <controller/CrapsReaders.h>
#include <gen/ErrorPass.h>

using namespace Cui;

//----------------------------------------------------------------

WindowPlayerArea::WindowPlayerArea()
    : PanelBase("WindowPlayerArea")
    , allPlayersView_(Layout::playerAreaHeight, Layout::playerAreaWidth)
    , onePlayerView_ (Layout::playerAreaHeight, Layout::playerAreaWidth)
{
    createWindow();
    allPlayersView_.init(pWin_);
    onePlayerView_.init (pWin_);
}

//----------------------------------------------------------------

WindowPlayerArea&
WindowPlayerArea::instance()
{
    static WindowPlayerArea wpa;
    return wpa;
}

//----------------------------------------------------------------

void
WindowPlayerArea::createWindow()
{
    newWindow(Layout::playerAreaHeight,           // In base class
              Layout::playerAreaWidth,
              Layout::playerAreaTopRow,
              Layout::playerAreaLeftCol);
}

//----------------------------------------------------------------

void
WindowPlayerArea::draw()
{
    werase(pWin_);

    drawExternalJunctions();
    drawInternalBorders();
    drawStaticContent();

    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------
//
// We need to touch up border junctions to mate with our internal
// lines. But the border is outside of our window. We ask 
// LayoutCrapsScreen to take of it.
//
void
WindowPlayerArea::drawExternalJunctions()
{
    if (currentFocus_ == OneOrAll::AllPlayers)
    {
        LayoutCrapsScreen::instance().eraseExternalJunctionsOnePlayer();
        LayoutCrapsScreen::instance().drawExternalJunctionsAllPlayers();
    }
    else
    {
        LayoutCrapsScreen::instance().eraseExternalJunctionsAllPlayers();
        LayoutCrapsScreen::instance().drawExternalJunctionsOnePlayer();
    }
}

//----------------------------------------------------------------

void
WindowPlayerArea::drawInternalBorders()
{
    // No need to erase, window was cleared before this
    if (currentFocus_ == OneOrAll::AllPlayers)
    {
        allPlayersView_.drawInternalBorders();
    }
    else
    {
        onePlayerView_.drawInternalBorders();
    }
}

//----------------------------------------------------------------
//
// Draw static field contents
//
void
WindowPlayerArea::drawStaticContent()
{
    if (currentFocus_ == OneOrAll::AllPlayers)
    {
        allPlayersView_.drawStaticContent();
    }
    else
    {
        onePlayerView_.drawStaticContent();
    }
}

//----------------------------------------------------------------
//
// Switch to OnePlayer View. If already showing, goto next player
//
void
WindowPlayerArea::nextPlayer()
{
    advancePlayer(true);
}

//----------------------------------------------------------------
//
// Switch to OnePlayer View. If already showing, goto prev player
//
void
WindowPlayerArea::prevPlayer()
{
    advancePlayer(false);
}

//----------------------------------------------------------------
//
// Switch to AllPlayers View. If already showing, just re-populate.
//
void
WindowPlayerArea::allPlayers()
{
    if (currentFocus_ == OneOrAll::OnePlayer)
    {
        currentFocus_ = OneOrAll::AllPlayers;
        werase(pWin_);
        drawExternalJunctions();
        drawInternalBorders();
        drawStaticContent();
    }
    
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------

void
WindowPlayerArea::advancePlayer(bool next)
{
    if (currentFocus_ == OneOrAll::AllPlayers)
    {
        currentFocus_ = OneOrAll::OnePlayer;
        werase(pWin_);
        drawExternalJunctions();
        drawInternalBorders();
        drawStaticContent();
        CuiUtils::transfer(pWin_);
        return;
    }
    
    // Else already in OnePlayer view, advance to next or prev player
    onePlayerView_.advancePlayer(next);
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------

void
WindowPlayerArea::onBetMade(
    const Craps::PlayerId& playerId,
    Craps::BetId           betId,
    BetName                betName,
    Gen::Money             contractAmount,
    Gen::Money             oddsAmount,
    unsigned               pivot)
{
    allPlayersView_.onBetMade(playerId,
                              betId,
                              betName,
                              contractAmount,
                              oddsAmount,
                              pivot);
    onePlayerView_.onBetMade(playerId,
                             betId,
                             betName,
                             contractAmount,
                             oddsAmount,
                             pivot);
}

//----------------------------------------------------------------

