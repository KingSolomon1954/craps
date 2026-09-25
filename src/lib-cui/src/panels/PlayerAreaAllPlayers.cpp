//----------------------------------------------------------------
//
// File: PlayerAreaAllPlayers.cpp
//
//----------------------------------------------------------------

#include <cui/panels/PlayerAreaAllPlayers.h>
#include <gen/Logger.h>

using namespace Cui;

//----------------------------------------------------------------

PlayerAreaAllPlayers::PlayerAreaAllPlayers(int height, int width)
    : winHeight_(height)
    , winWidth_(width)
{
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::drawInternalBorders()
{
    using L = Layout;
    
    // Vertical lines
    mvwvline(pWin_, 0, L::col1_2,  0, winHeight_);
    mvwvline(pWin_, 0, L::col2_3,  0, winHeight_);
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::setWindow(WINDOW* pWin)
{
    pWin_ = pWin;
}

//----------------------------------------------------------------
