//----------------------------------------------------------------
//
// File: WindowPlayerArea.cpp
//
//----------------------------------------------------------------

#include <cui/panels/WindowPlayerArea.h>
#include <cui/layouts/LayoutCrapsScreen.h>
#include <cui/core/CuiUtils.h>
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
// Switch to OnePlayer View. Show previously showing player.
// Called from MenuNavBarViews.
//
void
WindowPlayerArea::onePlayer()
{
    if (currentFocus_ == OneOrAll::AllPlayers)
    {
        currentFocus_ = OneOrAll::OnePlayer;
        draw();
    }
}

//----------------------------------------------------------------
//
// Switch to OnePlayer View. If already showing, goto next player
// Called from MenuNavBarViews.
//
void
WindowPlayerArea::nextPlayer()
{
    advancePlayer(true);
}

//----------------------------------------------------------------
//
// Switch to OnePlayer View. If already showing, goto prev player
// Called from MenuNavBarViews.
//
void
WindowPlayerArea::prevPlayer()
{
    advancePlayer(false);
}

//----------------------------------------------------------------
//
// Switch to AllPlayers View. If already showing, just re-populate.
// Called from MenuNavBarViews.
//
void
WindowPlayerArea::allPlayers()
{
    if (currentFocus_ == OneOrAll::OnePlayer)
    {
        currentFocus_ = OneOrAll::AllPlayers;
        draw();
    }
}

//----------------------------------------------------------------

void
WindowPlayerArea::advancePlayer(bool next)
{
    if (currentFocus_ == OneOrAll::OnePlayer)
    {
        onePlayerView_.advancePlayer(next);
        CuiUtils::transfer(pWin_);
    }
    else
    {
        currentFocus_ = OneOrAll::OnePlayer;
        draw();
    }
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

void
WindowPlayerArea::onBetResolved(
    const Craps::PlayerId& playerId,
    BetName                betName,
    Craps::BetId           betId,
    Gen::Money             amountWin,
    Gen::Money             amountLose)

{
    allPlayersView_.onBetResolved(playerId,
                                  betName,
                                  betId,
                                  amountWin,
                                  amountLose);
    onePlayerView_.onBetResolved(playerId,
                                  betName,
                                  betId,
                                  amountWin,
                                  amountLose);
}

//----------------------------------------------------------------

void
WindowPlayerArea::onBetChanged(
    const Craps::PlayerId& playerId,
    Craps::BetId           betId,
    BetName                betName,
    Gen::Money             contractAmount,
    Gen::Money             oddsAmount,
    unsigned               pivot)
{
    allPlayersView_.onBetChanged(playerId,
                                 betId,
                                 betName,
                                 contractAmount,
                                 oddsAmount,
                                 pivot);
    onePlayerView_.onBetChanged(playerId,
                                 betId,
                                 betName,
                                 contractAmount,
                                 oddsAmount,
                                 pivot);
}

//----------------------------------------------------------------

