//----------------------------------------------------------------
//
// File: CuiMain.cpp
//
//----------------------------------------------------------------

#include <cui/CuiMain.h>
#include <cui/CuiThread.h>
#include <cui/SurfaceManager.h>
#include <cui/screens/ScreenCrapsTable.h>
#include <cui/layouts/LayoutConsole.h>
#include <controller/Globals.h>
#include <cstdio>
#include <cstdlib>
#include <locale.h>

using namespace Cui;

//----------------------------------------------------------------

CuiMain&
CuiMain::instance()
{
    static CuiMain cm;
    return cm;
}

//----------------------------------------------------------------

void
CuiMain::init()
{
    using L = LayoutConsole;
    
    initLocale();
    initNcurses();
    
    if (!ensureMinimumTerminalSize(L::minRows, L::minCols))
    {
        endwin();
        std::fprintf(stderr, "Terminal too small; exiting.\n");
        fflush(stdout);
        std::string diag = "CuiMain::init(); Terminal too small, "
            "unable to continue. Resize manually to at least " +
            std::to_string(L::minRows) + " rows, " +
            std::to_string(L::minCols) + " cols.";
        throw std::runtime_error(diag);
    }
}

//----------------------------------------------------------------

void
CuiMain::initLocale()
{
    setlocale(LC_ALL, "");   // enable locale detection
    useUnicodePips_ = utf8_enabled();
    // LOG_DEBUG("useUnicodePips: " + std::to_string(useUnicodePips_));
}

//----------------------------------------------------------------

bool
CuiMain::utf8_enabled()
{
    const char* loc = setlocale(LC_CTYPE, nullptr);
    if (!loc) return false;
    std::string s(loc);
    return s.find("UTF-8") != std::string::npos || s.find("utf8") != std::string::npos;
}

//----------------------------------------------------------------

void
CuiMain::initNcurses()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    use_default_colors();
}

//----------------------------------------------------------------
//
// Sends an xterm-style window-ops escape sequence asking the terminal
// emulator to resize itself. Best-effort only — not all terminals honor
// this (most graphical emulators do; tmux/screen and some consoles
// don't).

void
CuiMain::requestTerminalResize(int rows, int cols)
{
    printf("\033[8;%d;%dt", rows, cols);
    fflush(stdout);
}

//----------------------------------------------------------------
//
// Displays a "too small" message and asks the user whether to resize.
// Returns true if the user agreed.
//
bool
CuiMain::promptUserToResize(int haveRows, int haveCols,
                            int needRows, int needCols)
{
    clear();
    mvprintw(0, 0, "Terminal is too small for %s", Gbl::appNameScreen);
    mvprintw(1, 0, "Have: %d rows x %d cols", haveRows, haveCols);
    mvprintw(2, 0, "Need: %d rows x %d cols", needRows, needCols);
    mvprintw(4, 0, "Resize automatically? [y/n]: ");
    refresh();

    int ch = getch();
    return (ch == 'y' || ch == 'Y');
}

//----------------------------------------------------------------
//
// Ensures the terminal is at least minRows x minCols, prompting the
// user to auto-resize if it's not. Returns true if the terminal meets
// the minimum (possibly after resizing); false means the caller should
// exit.
//
bool
CuiMain::ensureMinimumTerminalSize(int minRows, int minCols)
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    if (rows >= minRows && cols >= minCols)
    {
        return true;
    }

    if (!promptUserToResize(rows, cols, minRows, minCols))
    {
        return false;   // user declined
    }

    requestTerminalResize(minRows, minCols);

    // Give the terminal a moment to act on the request and deliver
    // SIGWINCH before we re-check.
    napms(200);
    
    if (is_term_resized(minRows, minCols))
    {
        resize_term(minRows, minCols);
    }
    
    getmaxyx(stdscr, rows, cols);
    if (rows >= minRows && cols >= minCols)
    {
        return true;
    }

    // Terminal didn't honor the request.
    clear();
    mvprintw(0, 0, "Unable to resize terminal automatically.");
    mvprintw(1, 0, "Please resize to at least %d x %d and restart.", minRows, minCols);
    refresh();
    return false;
}

//----------------------------------------------------------------

void
CuiMain::run()
{
    (void) CuiThread::instance();
    
    auto& mgr = SurfaceManager::instance();
    mgr.setSurface(&ScreenCrapsTable::instance());
}

//----------------------------------------------------------------

void
CuiMain::prepareForShutdown()
{
    SurfaceManager::instance().prepareForShutdown();
    endwin();
}

//----------------------------------------------------------------

CuiView&
CuiMain::getView()
{
    return CuiView::instance();
}

//----------------------------------------------------------------
