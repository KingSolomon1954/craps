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

using namespace Cui;

//----------------------------------------------------------------

WindowPlayerArea::WindowPlayerArea()
    : PanelBase("WindowPlayerArea")
{
    initPlayers();
    createWindow();
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
    using L = LayoutOnePlayer;

    // Horizontal lines
    mvwhline(pWin_, L::fieldBorderTopRow,    0, 0, Layout::playerAreaWidth);
    mvwhline(pWin_, L::crapsBorderTopRow,    0, 0, Layout::playerAreaWidth);
    mvwhline(pWin_, L::lineBetsBorderTopRow, 0, 0, Layout::playerAreaWidth);

    // Vertical lines
    mvwvline(pWin_, 0, L::col4_5,  0, L::rowsNumbers);
    mvwvline(pWin_, 0, L::col5_6,  0, L::rowsNumbers);
    mvwvline(pWin_, 0, L::col6_8,  0, L::rowsNumbers);
    mvwvline(pWin_, 0, L::col8_9,  0, L::rowsNumbers);
    mvwvline(pWin_, 0, L::col9_10, 0, L::rowsNumbers);
    mvwvline(pWin_, L::lineBetsBorderTopRow + 1, L::colComeDont2, 0, L::rowsLineBets);
    
    // Junctions
    mvwaddch(pWin_, L::fieldBorderTopRow,    L::col4_5,       ACS_BTEE);
    mvwaddch(pWin_, L::fieldBorderTopRow,    L::col5_6,       ACS_BTEE);
    mvwaddch(pWin_, L::fieldBorderTopRow,    L::col6_8,       ACS_BTEE);
    mvwaddch(pWin_, L::fieldBorderTopRow,    L::col8_9,       ACS_BTEE);
    mvwaddch(pWin_, L::fieldBorderTopRow,    L::col9_10,      ACS_BTEE);
    mvwaddch(pWin_, L::lineBetsBorderTopRow, L::colComeDont2, ACS_TTEE);
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
    mvwprintw(pWin_, 0, 0, "All Plavers View");
}

//----------------------------------------------------------------

void
WindowPlayerArea::drawStaticContentOnePlayer()
{
    // TODO
    mvwprintw(pWin_, 0, 0, "One Plaver View");
}

//----------------------------------------------------------------
//
// Updates all dynamic field content.
//
void
WindowPlayerArea::populate()
{
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
    mvwprintw(pWin_, 0, 0, "All Players Area");
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
    mvwprintw(pWin_, 0, 0, "One Player Area");
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
