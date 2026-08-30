//----------------------------------------------------------------
//
// File: WindowPlayerArea.cpp
//
//----------------------------------------------------------------

#include <cui/WindowPlayerArea.h>
#include <cui/CuiUtils.h>
#include <cui/ScreenCrapsTable.h>
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
    
    drawBorders();
    drawStaticContent();
    populate();
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------
//
// The outer border of PlayerArea is smooth and already drawn by
// ScreenCrapsTable. Here we modify the outer borders to place
// junctions that mate with our internal lines and fields.
//    
void
WindowPlayerArea::drawBorders()
{
    drawExternalJunctions();
    drawInternalBorders();
}

//----------------------------------------------------------------
//
// We need to touch up border junctions to mate with our internal
// lines. But the border is outside of our window. Ask ScreenCrapsTable
// to lends us its pWin_ so we can touch up border junctions.
//
void
WindowPlayerArea::drawExternalJunctions()
{
    WINDOW* pLendWin = ScreenCrapsTable::instance().lendWindow();
    if (currentFocus_ == OneOrAll::AllPlayers)
    {
        eraseExternalJunctionsOnePlayer(pLendWin);
        drawExternalJunctionsAllPlayers(pLendWin);
    }
    else
    {
        eraseExternalJunctionsAllPlayers(pLendWin);
        drawExternalJunctionsOnePlayer(pLendWin);
    }
    CuiUtils::transfer(pLendWin);
}

//----------------------------------------------------------------
//
// Fix up junctions for outer borders for all players view
//
void
WindowPlayerArea::drawExternalJunctionsAllPlayers(WINDOW* pLendWin)
{
    using L = LayoutCrapsScreen;
    using A = LayoutAllPlayers;
    
    // Has 4 junctions to apply
    mvwaddch(pLendWin, L::playerAreaBorderTopRow, A::col2, ACS_TTEE);
    mvwaddch(pLendWin, L::playerAreaBorderTopRow, A::col3, ACS_TTEE);
    mvwaddch(pLendWin, L::playerAreaBorderBotRow, A::col2, ACS_BTEE);
    mvwaddch(pLendWin, L::playerAreaBorderBotRow, A::col3, ACS_BTEE);
}

//----------------------------------------------------------------
//
// Restores player area junctions to a smooth rectangle
//
void
WindowPlayerArea::eraseExternalJunctionsAllPlayers(WINDOW* pLendWin)
{
    using L = LayoutCrapsScreen;
    using A = LayoutAllPlayers;
    
    // Has 4 junctions to restore
    mvwaddch(pLendWin, L::playerAreaBorderTopRow, A::col2, ACS_HLINE);
    mvwaddch(pLendWin, L::playerAreaBorderTopRow, A::col3, ACS_HLINE);
    mvwaddch(pLendWin, L::playerAreaBorderBotRow, A::col2, ACS_HLINE);
    mvwaddch(pLendWin, L::playerAreaBorderBotRow, A::col3, ACS_HLINE);
}

//----------------------------------------------------------------
//
// Fix up junctions for outer borders for one player view
//
void
WindowPlayerArea::drawExternalJunctionsOnePlayer(WINDOW* pLendWin)
{
    using L = LayoutCrapsScreen;
    using O = LayoutOnePlayer;

    // Top border
    mvwaddch(pLendWin, L::playerAreaBorderTopRow, O::col2, ACS_TTEE);
    mvwaddch(pLendWin, L::playerAreaBorderTopRow, O::col3, ACS_TTEE);
    mvwaddch(pLendWin, L::playerAreaBorderTopRow, O::col4, ACS_TTEE);
    mvwaddch(pLendWin, L::playerAreaBorderTopRow, O::col5, ACS_TTEE);
    mvwaddch(pLendWin, L::playerAreaBorderTopRow, O::col6, ACS_TTEE);

    // Bottom border
    mvwaddch(pLendWin, L::playerAreaBorderBotRow, O::lineBetSplitCol, ACS_BTEE);

    // Left border    
    mvwaddch(pLendWin, O::rowField,    L::playerAreaBorderLeftCol, ACS_LTEE);
    mvwaddch(pLendWin, O::rowCraps,    L::playerAreaBorderLeftCol, ACS_LTEE);
    mvwaddch(pLendWin, O::rowLineBets, L::playerAreaBorderLeftCol, ACS_LTEE);
    
    // Right border    
    mvwaddch(pLendWin, O::rowField,    L::playerAreaBorderRightCol, ACS_RTEE);
    mvwaddch(pLendWin, O::rowCraps,    L::playerAreaBorderRightCol, ACS_RTEE);
    mvwaddch(pLendWin, O::rowLineBets, L::playerAreaBorderRightCol, ACS_RTEE);
}

//----------------------------------------------------------------
//
// Restores table area junctions to a smooth square
//
void
WindowPlayerArea::eraseExternalJunctionsOnePlayer(WINDOW* pLendWin)
{
    using L = LayoutCrapsScreen;
    using O = LayoutOnePlayer;

    // Top border
    mvwaddch(pLendWin, L::playerAreaBorderTopRow, O::col2, ACS_HLINE);
    mvwaddch(pLendWin, L::playerAreaBorderTopRow, O::col3, ACS_HLINE);
    mvwaddch(pLendWin, L::playerAreaBorderTopRow, O::col4, ACS_HLINE);
    mvwaddch(pLendWin, L::playerAreaBorderTopRow, O::col5, ACS_HLINE);
    mvwaddch(pLendWin, L::playerAreaBorderTopRow, O::col6, ACS_HLINE);

    // Bottom border
    mvwaddch(pLendWin, L::playerAreaBorderBotRow, O::lineBetSplitCol, ACS_HLINE);

    // Left border    
    mvwaddch(pLendWin, O::rowField,    L::playerAreaBorderLeftCol, ACS_VLINE);
    mvwaddch(pLendWin, O::rowCraps,    L::playerAreaBorderLeftCol, ACS_VLINE);
    mvwaddch(pLendWin, O::rowLineBets, L::playerAreaBorderLeftCol, ACS_VLINE);
    
    // Right border    
    mvwaddch(pLendWin, O::rowField,    L::playerAreaBorderRightCol, ACS_VLINE);
    mvwaddch(pLendWin, O::rowCraps,    L::playerAreaBorderRightCol, ACS_VLINE);
    mvwaddch(pLendWin, O::rowLineBets, L::playerAreaBorderRightCol, ACS_VLINE);
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
        drawBorders();
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
        drawBorders();
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
