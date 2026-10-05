//---------------------------------------------------------------
//
// File: CuiUtils.cpp
//
//---------------------------------------------------------------

#include <cui/CuiUtils.h>
#include <cui/ColorManager.h>
#include <gen/ErrorPass.h>
#include <cassert>
#include <cwchar>

using namespace Cui;

/*-----------------------------------------------------------*//**

Move window contents to ncurses virtual backing store

@param[in] pWin
    The ncurses window of interest.

*/
void
CuiUtils::transfer(WINDOW* pWin)
{
    wnoutrefresh(pWin);
}

/*-----------------------------------------------------------*//**

Returns a new ncurses WINDOW centered on the given pWin.

@param[in] pWin
    The window to be centered in

@param[in] h
    Height of the window to be centered

@param[in] w
    Width of  the window to be centered

@return
    ncurses WINDOW* pointer
*/
WINDOW*
CuiUtils::makeCenteredWindow(WINDOW* pWin, int h, int w)
{
    int max_h, max_w;
    getmaxyx(pWin, max_h, max_w);

    // Clamp requested size to pWin screen size
    if (h > max_h) h = max_h;
    if (w > max_w) w = max_w;

    int start_y = (max_h - h) / 2;
    int start_x = (max_w - w) / 2;

    return newwin(h, w, start_y, start_x);
}

//----------------------------------------------------------------

WindowRect
CuiUtils::getWindowRect(WINDOW* pWin)
{
    int h, w;
    getmaxyx(pWin, h, w);
    int r, c;
    getyx(pWin, h, w);
    
    return {r,c,h,w};
}

/*-----------------------------------------------------------*//**

Returns a WindowRect with coordinates centered on the given pWin.

@param[in] pWin
    The window to be centered in

@param[in] size
    The window size (rows, cols)

@return
    WindowRect with coordinates centered on pWin
*/
WindowRect
CuiUtils::centerRect(WINDOW* pWin, WindowSize size)
{
    WindowRect rect = getWindowRect(pWin);

    // Clamp requested size to pWin screen size
    if (size.rows > rect.rows) size.rows = rect.rows;
    if (size.cols > rect.cols) size.cols = rect.cols;
    
    int start_r = (size.rows - rect.rows) / 2;
    int start_c = (size.cols - rect.cols) / 2;
    
    return {start_r, start_c, size.rows, size.cols};
}

//----------------------------------------------------------------
//
// Determine size of wide string taking into account any 
// embedded wchars.
//
int
CuiUtils::wstringWidth(const std::wstring& s)
{
    int msgW = 0;
    for (wchar_t ch : s)
    {
        int w = wcwidth(ch);
        if (w > 0) msgW += w;
    }
    return msgW;
}

//----------------------------------------------------------------

std::string
CuiUtils::playerName(const Craps::PlayerId& id)
{
    Gen::ErrorPass ep;
    std::string name;

    const auto rc = Ctrl::CrapsReaders::readPlayerName(id, name, ep);
    assert(rc == Gen::ReturnCode::Success);
    return name;
}

//----------------------------------------------------------------

short
CuiUtils::playerColorPair(const Craps::PlayerId& id)
{
    const auto pColor = ColorManager::instance().getPlayerColor(id);
    return ColorManager::instance().pair(*pColor);
}

//----------------------------------------------------------------

Craps::PlayerIds
CuiUtils::orderedPlayerIds()
{
    auto userPid   = userPlayerId();
    auto tableId   = activeTableId();
    auto playerIds = playersAtTable(tableId);

    Craps::PlayerIds orderedIds;  // Going to return this
    orderedIds.reserve(MaxPlayers);
    orderedIds.push_back(userPid);  // UserPlayer always gets index 0.

    // Preserve the existing order for everyone else.
    for (const auto& pid : playerIds)
    {
        if (pid != userPid)
        {
            orderedIds.push_back(pid);
        }

        if (orderedIds.size() == MaxPlayers) break;
    }
    return orderedIds;
}

//----------------------------------------------------------------

Craps::PlayerId
CuiUtils::userPlayerId()
{
    Gen::ErrorPass ep;
    Craps::PlayerId userPlayerId;
    
    auto rc = Ctrl::CrapsReaders::getUserPlayer(userPlayerId, ep);
    if (rc == Gen::ReturnCode::Fail)
    {
        ep.prepend("CuiUtils::userPlayer() unable to obtain; ");
        throw std::runtime_error(ep.description());
    }
    return userPlayerId;
}
    
//----------------------------------------------------------------

Craps::TableId
CuiUtils::activeTableId()
{
    Gen::ErrorPass ep;
    Craps::TableId tableId;
    
    auto rc = Ctrl::CrapsReaders::getActiveCrapsTable(tableId, ep);
    if (rc == Gen::ReturnCode::Fail)
    {
        ep.prepend("CuiUtils::activeTableId() unable to obtain; ");
        throw std::runtime_error(ep.description());
    }
    return tableId;
}

//----------------------------------------------------------------

Craps::PlayerIds
CuiUtils::playersAtTable(Craps::TableId tid)
{
    Gen::ErrorPass ep;
    Craps::PlayerIds pids;
    
    auto rc = Ctrl::CrapsReaders::readTablePlayers(tid, pids, ep);
    assert(pids.size() > 0);
    assert(pids.size() <= MaxPlayers);
    
    if (rc == Gen::ReturnCode::Fail)
    {
        ep.prepend("CuiUtils::playersAtTable() unable to obtain; ");
        throw std::runtime_error(ep.description());
    }
    return pids;
}

//----------------------------------------------------------------

CuiUtils::CuiBetName
CuiUtils::betToCuiBetName(BetName betName, unsigned pivot)
{
    switch (betName)
    {
        case BetName::PassLine:
            switch(pivot)
            {
                case 0: return CuiBetName::PassLine;
                case 4:
                case 5:
                case 6:
                case 8:
                case 9:
                case 10: return CuiBetName::PassLineOdds;
                default: assert(false);
            }
            
        case BetName::DontPass:
            switch(pivot)
            {
                case 0: return CuiBetName::DontPass;
                case 4:
                case 5:
                case 6:
                case 8:
                case 9:
                case 10: return CuiBetName::DontPassOdds;
                default: assert(false);
            }

        case BetName::Come:
            switch(pivot)
            {
                case 0:  return CuiBetName::Come;
                case 4:  return CuiBetName::Come4;
                case 5:  return CuiBetName::Come5;
                case 6:  return CuiBetName::Come6;
                case 8:  return CuiBetName::Come8;
                case 9:  return CuiBetName::Come9;
                case 10: return CuiBetName::Come10;
                default: assert(false);
            }

        case BetName::DontCome:
            switch(pivot)
            {
                case 0:  return CuiBetName::DontCome;
                case 4:  return CuiBetName::DontCome4;
                case 5:  return CuiBetName::DontCome5;
                case 6:  return CuiBetName::DontCome6;
                case 8:  return CuiBetName::DontCome8;
                case 9:  return CuiBetName::DontCome9;
                case 10: return CuiBetName::DontCome10;
                default: assert(false);
            }

        case BetName::Place:
            switch(pivot)
            {
                case 4:  return CuiBetName::Place4;
                case 5:  return CuiBetName::Place5;
                case 6:  return CuiBetName::Place6;
                case 8:  return CuiBetName::Place8;
                case 9:  return CuiBetName::Place9;
                case 10: return CuiBetName::Place10;
                default: assert(false);
            }

        case BetName::Buy:
            switch (pivot)
            {
                case 4:  return CuiBetName::Buy4;
                case 5:  return CuiBetName::Buy5;
                case 6:  return CuiBetName::Buy6;
                case 8:  return CuiBetName::Buy8;
                case 9:  return CuiBetName::Buy9;
                case 10: return CuiBetName::Buy10;
                default: assert(false);
            }

        case BetName::Lay:
            switch (pivot)
            {
                case 4:  return CuiBetName::Lay4;
                case 5:  return CuiBetName::Lay5;
                case 6:  return CuiBetName::Lay6;
                case 8:  return CuiBetName::Lay8;
                case 9:  return CuiBetName::Lay9;
                case 10: return CuiBetName::Lay10;
                default: assert(false);
            }

        case BetName::Hardway:
            switch(pivot)
            {
                case 4:  return CuiBetName::Hard4;
                case 6:  return CuiBetName::Hard6;
                case 8:  return CuiBetName::Hard8;
                case 10: return CuiBetName::Hard10;
                default: assert(false);
            }

        case BetName::CandE: return CuiBetName::CandE;

        case BetName::Field: return CuiBetName::Field;

        case BetName::AnyCraps: return CuiBetName::AnyCraps;

        case BetName::AnySeven: return CuiBetName::Any7;

        case BetName::Horn: return CuiBetName::Horn;

        default: assert(false);
    }
}

//----------------------------------------------------------------
