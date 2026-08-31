//----------------------------------------------------------------
//
// File: WindowPlayerArea.cpp
//
//----------------------------------------------------------------

#include <cui/panels/WindowPlayerArea.h>
#include <cui/layouts/LayoutCrapsScreen.h>
#include <cui/layouts/LayoutPlayerArea.h>
#include <cui/CuiUtils.h>
#include <controller/CrapsReaders.h>
#include <gen/ErrorPass.h>
#include <gen/Logger.h>

using namespace Cui;

//----------------------------------------------------------------

WindowPlayerArea::WindowPlayerArea()
{
    initPlayers();
    newWindow(Layout::playerAreaHeight,           // In base class
              Layout::playerAreaWidth,
              Layout::playerAreaTopRow,
              Layout::playerAreaLeftCol,
              "WindowPlayerArea");
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
WindowPlayerArea::initPlayers()
{
    Gen::ErrorPass ep;
    auto rc = Ctrl::CrapsReaders::getUserPlayer(curPlayerId_, ep);
    if (rc == Gen::ReturnCode::Fail)
    {
        ep.prepend("WindowPlayerArea::initPlayers(): unable to init; ");
        throw std::runtime_error(ep.diag);
    }

    rc = Ctrl::CrapsReaders::getActiveCrapsTable(tableId_, ep);
    if (rc == Gen::ReturnCode::Fail)
    {
        ep.prepend("WindowPlayerArea::initPlayers() unable to init; ");
        throw std::runtime_error(ep.diag);
    }

    rc = Ctrl::CrapsReaders::readTablePlayers(tableId_, playerIds_, ep);
    assert(playerIds_.size() > 0);
}

//----------------------------------------------------------------

void
WindowPlayerArea::draw()
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
        drawInternalBordersAllPlayers();
    }
    else
    {
        drawInternalBordersOnePlayer();
    }
}

//----------------------------------------------------------------

void
WindowPlayerArea::drawInternalBordersAllPlayers()
{
    // TODO
}

//----------------------------------------------------------------

void
WindowPlayerArea::drawInternalBordersOnePlayer()
{
    // TODO
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
        drawStaticContentAllPlayers();
    }
    else
    {
        drawStaticContentOnePlayer();
    }
}

//----------------------------------------------------------------

void
WindowPlayerArea::drawStaticContentAllPlayers()
{
    // TODO
}

//----------------------------------------------------------------

void
WindowPlayerArea::drawStaticContentOnePlayer()
{
    // TODO
}

//----------------------------------------------------------------
//
// Updates all dynamic field content.
//
void
WindowPlayerArea::populate()
{
    mvwprintw(pWin_, 0, 0, "Player Area View here");
    if (currentFocus_ == OneOrAll::AllPlayers)
    {
        populateAllPlayers();
    }
    else
    {
        populateOnePlayer();
    }
}

//----------------------------------------------------------------
//
// Updates all dynamic fields in this view.
//
void
WindowPlayerArea::populateAllPlayers()
{
    // Updates all dynamic fields in this view.
    // TODO
    // updatePassLineBets()
    // updateFieldBets()
    // updateFieldBets()
    // updateXxx()
    // ...
}

//----------------------------------------------------------------
//
// Updates all dynamic fields in this view
//
void
WindowPlayerArea::populateOnePlayer()
{
    // TODO
    // updatePassLineBets()
    // updateFieldBets()
    // updateFieldBets()
    // updateXxx()
    // ...
}

//----------------------------------------------------------------

Craps::PlayerId
WindowPlayerArea::getNextPlayerId(const Craps::PlayerId& pid) const
{
    if (playerIds_.empty())
    {
        throw std::runtime_error("playerIds_ is empty");
    }

    auto it = std::find(playerIds_.begin(), playerIds_.end(), pid);

    if (it == playerIds_.end())
    {
        throw std::runtime_error("Current player was not found");
    }

    ++it;

    // Wrap around from the last player to the first.
    if (it == playerIds_.end())
    {
        it = playerIds_.begin();
    }

    return *it;
}

//----------------------------------------------------------------

Craps::PlayerId
WindowPlayerArea::getPrevPlayerId(const Craps::PlayerId& pid) const
{
    if (playerIds_.empty())
    {
        throw std::runtime_error("playerIds_ is empty");
    }

    auto it = std::find(playerIds_.begin(), playerIds_.end(), pid);

    if (it == playerIds_.end())
    {
        throw std::runtime_error("Current player was not found");
    }

    // Wrap around from the first player to the last.
    if (it == playerIds_.begin())
    {
        it = playerIds_.end();
    }

    --it;

    return *it;
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
    
    populateAllPlayers();
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
        populateOnePlayer();
        CuiUtils::transfer(pWin_);
        return;
    }
    
    // Else already in OnePlayer view, advance to next or prev player

    Craps::PlayerId pid;
    if (next)
    {
        pid = getNextPlayerId(curPlayerId_);
    }
    else
    {
        pid = getPrevPlayerId(curPlayerId_);
    }
    if (pid == curPlayerId_)
    {
        return; // No next or prev player to display, stay on current
    }
    
    curPlayerId_ = pid;
    populateOnePlayer();
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------

void
WindowPlayerArea::onPlayerJoined(const Craps::PlayerId& pid)
{
    // TODO
}

//----------------------------------------------------------------

void
WindowPlayerArea::onPlayerLeft(const Craps::PlayerId& pid)
{
    // TODO
}

//----------------------------------------------------------------
