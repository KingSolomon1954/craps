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
PlayerAreaOnePlayer::init(WINDOW* pWin)
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

void
PlayerAreaOnePlayer::drawStaticContent()
{
    drawNumberHeading();
    drawNumberLabels();
    drawFieldLabels();
    drawCandELabels();
    drawLineBetLabels();
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::drawNumberHeading()
{
    constexpr int row = 0;
    
#if 0
    PlayerColor pc;
    const auto pColor = ColorManager::instance().getPlayerColor(playerId_);
    assert(pColor.has_value());
    if (pColor.has_value())
    {
        pc = *pColor;
    }
    
    wattron(pWin_, COLOR_PAIR(ColorManager::instance().pair(pc)));
    mvwaddstr(pWin_, 0, 1, n.c_str());
    wattroff(pWin_, COLOR_PAIR(ColorManager::instance().pair(pc)));
#endif
    
    mvwaddch (pWin_, row, 13, '4');
    mvwaddch (pWin_, row, 23, '5');
    mvwaddch (pWin_, row, 33, '6');
    mvwaddch (pWin_, row, 43, '8');
    mvwaddch (pWin_, row, 53, '9');
    mvwaddstr(pWin_, row, 63, "10");
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::drawNumberLabels()
{
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::drawFieldLabels()
{
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::drawCandELabels()
{
}

//----------------------------------------------------------------

void
PlayerAreaOnePlayer::drawLineBetLabels()
{
}

//----------------------------------------------------------------
