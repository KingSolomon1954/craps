//----------------------------------------------------------------
//
// File: PlayerAreaOnePlayer.cpp
//
//----------------------------------------------------------------

#include <cui/panels/PlayerAreaOnePlayer.h>
#include <gen/Logger.h>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

PlayerAreaOnePlayer::PlayerAreaOnePlayer(int height, int width)
    : winHeight_(height)
    , winWidth_(width)
{
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::setWindow(WINDOW* pWin)
{
    pWin_ = pWin;
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::drawInternalBorders()
{
    using L = Layout;

    // Horizontal lines
    mvwhline(pWin_, L::fieldBorderTopRow,    0, 0, winWidth_);
    mvwhline(pWin_, L::crapsBorderTopRow,    0, 0, winWidth_);
    mvwhline(pWin_, L::lineBetsBorderTopRow, 0, 0, winWidth_);

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
