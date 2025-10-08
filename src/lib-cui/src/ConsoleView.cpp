//----------------------------------------------------------------
//
// File: ConsoleView.cpp
//
//----------------------------------------------------------------

#include <cui/ConsoleView.h>
#include <cassert>
#include <chrono>
#include <locale.h>
#include <controller/Globals.h>
#include <controller/GameEvent.h>
#include <controller/GameController.h>
#include <cui/Screen.h>
#include <cui/ScreenCrapsTable.h>
#include <gen/Logger.h>

using namespace Cui;
using namespace std::chrono_literals;

//----------------------------------------------------------------

ConsoleView::ConsoleView()
{
    setlocale(LC_ALL, "");   // enable locale detection
    useUnicodePips = utf8_enabled();
    LOG_DEBUG("useUnicodePips: " + std::to_string(useUnicodePips));
}

//----------------------------------------------------------------

ConsoleView*
ConsoleView::instance()
{
    static ConsoleView consoleView;
    return &consoleView;
}

//----------------------------------------------------------------

ConsoleView::~ConsoleView()
{
    prepareForShutdown();
}

//----------------------------------------------------------------

void
ConsoleView::init()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    use_default_colors();

    // Starting screen for the game
    setScreen(ScreenCrapsTable::instance());

    inputThread_ = std::thread(&ConsoleView::inputThreadFunc, this);
}

//----------------------------------------------------------------

void
ConsoleView::prepareForShutdown()
{
    running_ = false;
    if (inputThread_.joinable())
    {
        inputThread_.join();
    }

    {
        std::lock_guard<std::mutex> lk(stackMx_);
        for (auto* s : stack_) s->onDetach();
        stack_.clear();
    }
    endwin();
}

//----------------------------------------------------------------

void
ConsoleView::setScreen(Screen* pScreen)
{
    std::lock_guard<std::mutex> lock(stackMx_);
    for (auto* s : stack_) s->onDetach();
    stack_.clear();
    stack_.push_back(pScreen);
    pScreen->onAttach();
    redrawUnlocked();
}

//----------------------------------------------------------------

void
ConsoleView::pushScreen(Screen* pScreen)
{
    std::lock_guard<std::mutex> lock(stackMx_);

    if (pScreen->type() == Screen::ScreenType::Full)
    {
        // Pause all, detach top, clear stack
        if (!stack_.empty())
        {
            stack_.back()->onPause();
        }
        for (auto* s : stack_) s->onDetach();
        stack_.clear();
    }
    else
    {
        if (!stack_.empty()) stack_.back()->onPause();
    }

    stack_.push_back(pScreen);
    pScreen->onAttach();
    redrawUnlocked();
}

//----------------------------------------------------------------

void
ConsoleView::popScreen()
{
    std::lock_guard<std::mutex> lock(stackMx_);
    if (stack_.empty()) return;

    auto* top = stack_.back();
    stack_.pop_back();
    top->onDetach();

    if (!stack_.empty())
    {
        stack_.back()->onResume();
    }
    redrawUnlocked();
}

//----------------------------------------------------------------

void
ConsoleView::redraw()
{
    std::lock_guard<std::mutex> lock(stackMx_);
    redrawUnlocked();
}

//----------------------------------------------------------------

void
ConsoleView::redrawUnlocked()
{
    if (stack_.empty()) return;

    // Find bottom-most full screen
    auto it = std::find_if(stack_.begin(), stack_.end(),
          [](Screen* s){ return s->type() == Screen::ScreenType::Full; });

    if (it != stack_.end())
    {
        werase(stdscr);
        (*it)->draw(); // Draw base full screen
        ++it;
    }

    for (; it != stack_.end(); ++it)  // Draw overlays in order
    {
        (*it)->draw();
    }
    doupdate();
}

//----------------------------------------------------------------

void
ConsoleView::inputThreadFunc()
{
    nodelay(stdscr, TRUE);
    while (running_)
    {
//      LOG_TRACE("ConsoleView::inputThreadFunc() before wgetch()");
        int ch = wgetch(stdscr);
//      LOG_TRACE("ConsoleView::inputThreadFunc() after wgetch(" + std::to_string(ch) + ")");
        if (ch == ERR)
        {
//          LOG_TRACE("ConsoleView::inputThreadFunc() sleeping");
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        std::lock_guard<std::mutex> lk(stackMx_);
        if (!stack_.empty())
        {
            LOG_TRACE("ConsoleView::inputThreadFunc() calling handlekey(" + std::to_string(ch) + ")");
            stack_.back()->handleKey(ch);
        }
    }
}



//----------------------------------------------------------------
//
// Create a new window centered on the screen
// @param h Desired height
// @param w Desired width
// @return A new WINDOW* positioned at the center
//
WINDOW*
ConsoleView::makeCenteredWindow(int h, int w)
{
    int max_h, max_w;
    getmaxyx(stdscr, max_h, max_w);

    // Clamp requested size to screen size
    if (h > max_h) h = max_h;
    if (w > max_w) w = max_w;

    int start_y = (max_h - h) / 2;
    int start_x = (max_w - w) / 2;

    return newwin(h, w, start_y, start_x);
}

//----------------------------------------------------------------

void
ConsoleView::showSuccess(
    Ctrl::EventType orgEventType,
    uint64_t correlationId)
{
    // TODO
}

//----------------------------------------------------------------
//
// Show Error Dialog. Overrides ViewInterface.
//
void
ConsoleView::showErrorDialog(Ctrl::EventType    orgEventType,
                             uint64_t           correlationId,
                             const std::string& diag)
{
    // Inform all UI listeners
    // Notify menus/screens of an error, puts up an ErrorDialog
    // uiEvent.correlationId = ev->correlationId;
    // uiEvent.orgEventType = ev->orgEventType;
    // uiEvent.diag = ev->diag;
    // uiEventMgr->publish(uiEvent);
}

//----------------------------------------------------------------

void
ConsoleView::showMakeBetSuccess(
    Craps::BetId betId,
    uint64_t correlationId)
{
    // TODO
}

//----------------------------------------------------------------

void
ConsoleView::showMakeOddsBetSuccess(
    Craps::BetId betId,
    uint64_t correlationId)
{
    // TODO
}

//----------------------------------------------------------------

void
ConsoleView::showMakeBetAutoSuccess(
    Craps::BetId betId,
    uint64_t correlationId)
{
    // TODO
}

//----------------------------------------------------------------

void
ConsoleView::showMakeBetAutoError(
    Ctrl::EventType orgEventType,
    uint64_t correlationId,
    const Craps::PlayerId& playerId,
    const std::string& diag)
{
    // TODO
}

//----------------------------------------------------------------

void
ConsoleView::showRollDiceCountDown(
    uint64_t correlationId,
    int      numSeconds)
{
    // TODO
}

//----------------------------------------------------------------

void
ConsoleView::showRollDiceAnimation(uint64_t correlationId)
{
    // TODO
}

//----------------------------------------------------------------

void
ConsoleView::showProgramExit()
{
    // TODO
}

//----------------------------------------------------------------

bool
ConsoleView::utf8_enabled()
{
    const char* loc = setlocale(LC_CTYPE, nullptr);
    if (!loc) return false;
    std::string s(loc);
    return s.find("UTF-8") != std::string::npos || s.find("utf8") != std::string::npos;
}

//----------------------------------------------------------------
