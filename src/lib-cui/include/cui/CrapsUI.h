//----------------------------------------------------------------
//
// File: CrapsUi.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/ConsoleManager.h>

namespace Cui
{

class ConsoleManager;      // fwd
class ScreenCrapsTable;    // fwd
class MenuBetting;         // fwd
class StatsSessionHistory; // fwd
    
class CrapsUI
{
public:
    CrapsUI() = default;
   ~CrapsUI() = default;
   
    void init();
    void run();
    
private:
    ConsoleManager*      pConsoleMgr_;
    ScreenCrapsTable*    pScreenCrapsTable_;
    MenuBetting*         pMenuBetting_;
    StatsSessionHistory* pStatsSessionHistory_;

    void createContentWindows();
};


/*-----------------------------------------------------------*//**

@class CrapsUi

@brief Specialized surface type for full screens

@li Provides the generic UI surface contract for full screens.

*/

} // namespace Cui

//----------------------------------------------------------------



#if 0

class CrapsUI
{
private:
    ConsoleManager console_;

    std::unique_ptr<ScreenCrapsTable> crapsTable_;
    std::unique_ptr<MenuBetting> bettingMenu_;
    std::unique_ptr<StatsSessionHistory> sessionHistory_;
};





    // TODO turn into window classes each owning an ncurses WINDOW.
    #include <vector>
    struct Windows
    {
        WINDOW *header      = nullptr, *animation   = nullptr,
               *rollHistory = nullptr, *houseBrief  = nullptr,
               *playerArea  = nullptr, *playerBrief = nullptr,
               *message     = nullptr;
    };

#endif
