//----------------------------------------------------------------
//
// File: CuiMain.cpp
//
//----------------------------------------------------------------

#include <cui/CuiMain.h>
#include <cui/SurfaceManager.h>
#include <cui/screens/ScreenCrapsTable.h>
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
    initLocale();
    initNcurses();
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
    // TODO handle size of user window
    // int termRows, termCols;
    // getmaxyx(stdscr, termRows, termCols);
    // if (is_term_resized(41, 99)) {
    //     resize_term(41, 99);
    //    // Recalculate visible regions and redraw
    
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    use_default_colors();
}

//----------------------------------------------------------------

void
CuiMain::run()
{
    auto& mgr = SurfaceManager::instance();
    mgr.setSurface(&ScreenCrapsTable::instance());
    SurfaceManager::instance().run();
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
