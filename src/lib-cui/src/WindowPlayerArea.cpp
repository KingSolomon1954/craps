a//----------------------------------------------------------------
//
// File: WindowPlayerArea.cpp
//
//----------------------------------------------------------------

#include <cui/WindowPlayerArea.h>
#include <cui/CuiUtils.h>
#include <controller/CrapsReaders.h>
#include <gen/ErrorPass.h>
#include <gen/Logger.h>
#include <ncurses.h>

using namespace Cui;

//----------------------------------------------------------------

WindowPlayerArea::WindowPlayerArea()
{
    initPlayers();
    pWindow_ = newwin(
        Layout::playerAreaHeight,
        Layout::playerAreaWidth,
        Layout::playerAreaTopRow,
        Layout::playerAreaLeftCol);
    
    assert(pWindow_ != nullptr);
}

//----------------------------------------------------------------

void
WindowPlayerArea::initPlayers()
{
    auto rc = Ctrl::CrapsReaders::getUserPlayer(curPlayerId_, ep);
    if (rc == Gen::ReturnCode::Fail)
    {
        ep.prepend("WindowPlayerArea::ScreenCrapsTable(): unable to init; ");
        throw std::runtime_error(ep.diag);
    }

    rc = Ctrl::CrapsReaders::getActiveCrapsTable(tableId_, ep);
    if (rc == Gen::ReturnCode::Fail)
    {
        ep.prepend("ScreenCrapsTable::ScreenCrapsTable(): unable to init; ");
        throw std::runtime_error(ep.diag);
    }

    rc = Ctrl::CrapsReaders::readTablePlayers(tableId_, playerIds_, ep);
    assert(playerIds_.size() > 0);
}

//----------------------------------------------------------------

WindowWindowPlayerArea::~WindowPlayerArea()
{
    if (pWindow_ != nullptr)
    {
        delwin(pWindow_);
    }
}

//----------------------------------------------------------------

void
WindowPlayerArea::draw()
{
    werase(pWindow_);
    
    drawBorders();
    drawStaticContent();
    populate();
    CuiUtils::transfer(pWindow_);
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

void
WindowPlayerArea::drawExternalJunctions()
{
    if (currentFocus_ == OneOrAll::AllPlayers)
    {
        eraseExternalJunctionsOnePlayer();
        drawExternalJunctionsAllPlayers();
    }
    else
    {
        eraseExternalJunctionsAllPlayers();
        drawExternalJunctionsOnePlayer();
    }
}

//----------------------------------------------------------------
//
// Write junctions for outer borders for all players view
//
void
WindowPlayerArea::drawExternalJunctionsAllPlayers()
{
    using L = LayoutCrapsScreen;;
    using A = LayoutAllPlayers;
    
    // Has 4 junctions to apply
    mvaddch(L::playerAreaBorderTopRow, A::col2, ACS_TTEE);
    mvaddch(L::playerAreaBorderTopRow, A::col3, ACS_TTEE);
    mvaddch(L::playerAreaBorderBotRow, A::col2, ACS_BTEE);
    mvaddch(L::playerAreaBorderBotRow, A::col3, ACS_BTEE);
}

//----------------------------------------------------------------
//
// Restores player area junctions to a smooth rectangle
//
void
WindowPlayerArea::eraseExternalJunctionsAllPlayers()
{
    using L = LayoutCrapsScreen;
    using A = LayoutAllPlayers;
    
    // Has 4 junctions to restore
    mvaddch(L::playerAreaBorderTopRow, A::col2, ACS_HLINE);
    mvaddch(L::playerAreaBorderTopRow, A::col3, ACS_HLINE);
    mvaddch(L::playerAreaBorderBotRow, A::col2, ACS_HLINE);
    mvaddch(L::playerAreaBorderBotRow, A::col3, ACS_HLINE);
}

//----------------------------------------------------------------
//
// Fix up junctions for outer borders for one player view
//
void
WindowPlayerArea::drawExternalJunctionsOnePlayer()
{
    using L = LayoutCrapsScreen;
    using O = LayoutOnePlayer;

    // Top border
    mvaddch(L::playerAreaBorderTopRow, O::col2, ACS_TTEE);
    mvaddch(L::playerAreaBorderTopRow, O::col3, ACS_TTEE);
    mvaddch(L::playerAreaBorderTopRow, O::col4, ACS_TTEE);
    mvaddch(L::playerAreaBorderTopRow, O::col5, ACS_TTEE);
    mvaddch(L::playerAreaBorderTopRow, O::col6, ACS_TTEE);

    // Bottom border
    mvaddch(L::playerAreaBorderBotRow, O::lineBetSplitCol, ACS_BTEE);

    // Left border    
    mvaddch(O::rowField,    L::playerAreaBorderLeftCol, ACS_LTEE);
    mvaddch(O::rowCraps,    L::playerAreaBorderLeftCol, ACS_LTEE);
    mvaddch(O::rowLineBets, L::playerAreaBorderLeftCol, ACS_LTEE);
    
    // Right border    
    mvaddch(O::rowField,    L::playerAreaBorderRightCol, ACS_RTEE);
    mvaddch(O::rowCraps,    L::playerAreaBorderRightCol, ACS_RTEE);
    mvaddch(O::rowLineBets, L::playerAreaBorderRightCol, ACS_RTEE);
}

//----------------------------------------------------------------
//
// Restores table area junctions to a smooth square
//
void
WindowPlayerArea::eraseExternalJunctionsOnePlayer()
{
    using L = LayoutCrapsScreen;
    using O = LayoutOnePlayer;

    // Top border
    mvaddch(L::playerAreaBorderTopRow, O::col2, ACS_HLINE);
    mvaddch(L::playerAreaBorderTopRow, O::col3, ACS_HLINE);
    mvaddch(L::playerAreaBorderTopRow, O::col4, ACS_HLINE);
    mvaddch(L::playerAreaBorderTopRow, O::col5, ACS_HLINE);
    mvaddch(L::playerAreaBorderTopRow, O::col6, ACS_HLINE);

    // Bottom border
    mvaddch(L::playerAreaBorderBotRow, O::lineBetSplitCol, ACS_HLINE);

    // Left border    
    mvaddch(O::rowField,    L::playerAreaBorderLeftCol, ACS_VLINE);
    mvaddch(O::rowCraps,    L::playerAreaBorderLeftCol, ACS_VLINE);
    mvaddch(O::rowLineBets, L::playerAreaBorderLeftCol, ACS_VLINE);
    
    // Right border    
    mvaddch(O::rowField,    L::playerAreaBorderRightCol, ACS_VLINE);
    mvaddch(O::rowCraps,    L::playerAreaBorderRightCol, ACS_VLINE);
    mvaddch(O::rowLineBets, L::playerAreaBorderRightCol, ACS_VLINE);
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
    mvwprintw(pWindow_, 0, 0, "Player Area View here");
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
WindowPlayerArea::getNextPlayerId(const Craps::PlayerId& pid)
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
WindowPlayerArea::getPrevPlayerId(const Craps::PlayerId& pid)
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
    if (it == playerIds.begin())
    {
        it = playerIds_.end();
    }

    --it;

    return *it;
}

//----------------------------------------------------------------

void
WindowPlayerArea::nextPlayer()
{
    advancePlayer(true);
}

//----------------------------------------------------------------

void
WindowPlayerArea::prevPlayer()
{
    advancePlayer(false);
}
    
//----------------------------------------------------------------
void
WindowPlayerArea::advancePlayer(bool next)
{
    if (currentFocus_ == OneOrAll::AllPlayers)
    {
        currentFocus_ = OneOrAll::OnePlayer;
        werase(pWindow_);
        drawBorders();
        drawStaticContent();
        populateOnePlayer();
        CuiUtils::transfer(pWindow_);
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
    CuiUtils::transfer(pWindow_);
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

#if 0

void WindowPlayerArea::updateBankroll()
{
    mvwprintw(pWindow_, row, col, "%-10s", formatted);
}

#endif

//----------------------------------------------------------------
