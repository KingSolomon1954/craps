//----------------------------------------------------------------
//
// File: CuiMain.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/CuiView.h>

namespace Cui
{
    
class CuiMain
{
public:
    CuiMain();
   ~CuiMain() = default;
   
    static CuiMain& instance();
    void prepareForShutdown();
    CuiView& getView();
    
private:
    bool useUnicodePips_ = false;
    
    void init();
    void run();
    void initLocale();
    void initNcurses();
    bool utf8_enabled();
    void requestTerminalResize    (int rows,     int cols);
    bool promptUserToResize       (int haveRows, int haveCols,
                                   int needRows, int needCols);
    bool ensureMinimumTerminalSize(int minRows,  int minCols);
};

/*-----------------------------------------------------------*//**

@class CuiMain

@brief Main entry point for CUI library

@li The entry point into CUI processing.
@li Ncurses initializaiton
@li Controller library pulls in this class to start it off

*/

} // namespace Cui

//----------------------------------------------------------------

