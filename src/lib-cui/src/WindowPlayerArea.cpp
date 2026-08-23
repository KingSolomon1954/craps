a//----------------------------------------------------------------
//
// File: WindowPlayerArea.cpp
//
//----------------------------------------------------------------

#include <ncurses.h>
#include <cassert>

/*
update()
    Incrementally modify selected content.

transfer()
    Move window contents to the ncurses virtual/backing screen.

doupdate()
    ncurses operation to update physical terminal screen
*/
    
//----------------------------------------------------------------

PlayerArea::PlayerArea()
{
    pWindow_ = newwin(
        Layout::playerAreaHeight,
        Layout::playerAreaWidth,
        Layout::playerAreaTopRow,
        Layout::playerAreaLeftCol);
    
    assert(pWindow_ != nullptr);
}

//----------------------------------------------------------------

WindowPlayerArea::~WindowPlayerArea()
{
    if (pWindow_ != nullptr)
    {
        delwin(pWindow_);
    }
}

//----------------------------------------------------------------
//
// Move window contents to ncurses virtual backing store
//
void
WindowPlayerArea::transfer();
{
    wnoutrefresh(pWindow_);
}

//----------------------------------------------------------------

void
WindowPlayerArea::draw()
{
    werase(pWindow_);
    drawBorders();
    populate();
    transfer();
}

//----------------------------------------------------------------
//
// The outer border of PlayerArea is smooth and already drawn by
// ScreenCrapsTable. Here we modify the outer borders to place
// junctions that mate with our internal lines and fields.
// And we draw any static inner separators and borders.
//    
void
WindowPlayerArea::drawBorders()
{
    drawExternalJunctions();
    drawInternalBorders();
    drawStaticContent();
}

//----------------------------------------------------------------

void
WindowPlayerArea::drawExternalJunctions()
{
    if (playerArea_ == PlayerArea::AllPlayers)
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
    playerArea_ == PlayerArea::AllPlayers ?
        drawInternalBordersAllPlayers() :
        drawInternalBordersOnePlayer();
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

void
WindowPlayerArea::drawStaticContent()
{
    playerArea_ == PlayerArea::AllPlayers ?
        drawStaticContentAllPlayers() :
        drawStaticContentOnePlayer();
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
// Populates displayable content of all dynamic fields
//
void
WindowPlayerArea::populate()
{
    mvwprintw(pWindow_, 0, 0, "Player Area View here");
    
    playerArea_ == PlayerArea::AllPlayers ?
        populateAllPlayers() :
        populateOnePlayer();
}

//----------------------------------------------------------------

void
WindowPlayerArea::populateAllPlayers()
{
    // TODO
}

//----------------------------------------------------------------

void
WindowPlayerArea::populateOnePlayer()
{
    // TODO
}

//----------------------------------------------------------------
//
// TODO This changes to support next and previous instead of cycle
// TODO hook into menu processing
//
void
WindowPlayerArea::cyclePlayerArea()
{
    if (playerArea_ == PlayerArea::AllPlayers)
    {
        // Switch to first player
        playerArea_ = PlayerArea::OnePlayer;
        onePlayerIndex_ = 0;
    }
    else
    {
        // Already in OnePlayer mode
        if (onePlayerIndex_ + 1 < playerIds_.size())
        {
            ++onePlayerIndex_;
        }
        else
        {
            // After last player → back to allPlayers
            playerArea_ = PlayerArea::AllPlayers;
            onePlayerIndex_ = 0;
        }
    }
}

//----------------------------------------------------------------

