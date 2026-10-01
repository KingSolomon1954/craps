//----------------------------------------------------------------
//
// File: CuiUtils.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/CuiStructs.h>
#include <controller/CrapsReaders.h>
#include <craps/CrapsTypes.h>
#include <craps/EnumBetName.h>
#include <ncurses.h>
#include <string>
#include <wchar.h>

//----------------------------------------------------------------

namespace Cui {

class CuiUtils
{
public:
    enum class CuiBetName
    {
        Place4,    Place5,    Place6,    Place8,    Place9,    Place10,
        Come4,     Come5,     Come6,     Come8,     Come9,     Come10,
        ComeOdds4, ComeOdds5, ComeOdds6, ComeOdds8, ComeOdds9, ComeOdds10,
        DontCome4, DontCome5, DontCome6, DontCome8, DontCome9, DontCome10,
        DontOdds4, DontOdds5, DontOdds6, DontOdds8, DontOdds9, DontOdds10,
        Buy4,      Buy5,      Buy6,      Buy8,      Buy9,      Buy10,
        Lay4,      Lay5,      Lay6,      Lay8,      Lay9,      Lay10,
        Hard4,     Hard6,     Hard8,     Hard10,    Field,
        AnyCraps,  CandE,     Horn,      Any7,      World,
        Come,      DontCome,
        PassLine,  PassLineOdds,
        DontPass,  DontPassOdds
    };

    /// @name CUI Utilities
    /// @{
    static void transfer(WINDOW* pWin);
    static WINDOW* makeCenteredWindow(WINDOW* pWin, int h, int w);
    static WindowRect centerRect(WINDOW* pWin, WindowSize size);
    static WindowRect getWindowRect(WINDOW* pWin);
    static int wstringWidth(const std::wstring& msg);
    static std::string playerName(const Craps::PlayerId& id);
    static short playerColorPair (const Craps::PlayerId& id);
    static Ctrl::CrapsReaders::PlayerIds orderedPlayerIds();
    static Craps::PlayerId userPlayerId();
    static Craps::TableId activeTableId();
    static Ctrl::CrapsReaders::PlayerIds playersAtTable(Craps::TableId tid);
    static CuiBetName betToCuiBetName(BetName betName, unsigned pivot);
    /// @}
};

/*-----------------------------------------------------------*//**

@class CuiUtils

@brief Some useful static functions for working with ncurses.

*/

//----------------------------------------------------------------

}  // namespace Cui
