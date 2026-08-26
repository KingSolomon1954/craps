//----------------------------------------------------------------
//
// File: ConsoleManager.cpp
//
//----------------------------------------------------------------

#include <cui/ConsoleManager.h>
#include <cui/Surface.h>
#include <controller/Globals.h>
#include <controller/GameEvent.h>
#include <controller/GameController.h>
#include <gen/Logger.h>
#include <cassert>
#include <chrono>
#include <locale.h>

using namespace Cui;
using namespace std::chrono_literals;

//----------------------------------------------------------------

ConsoleManager::ConsoleManager()
{
    setlocale(LC_ALL, "");   // enable locale detection
    useUnicodePips_ = utf8_enabled();
    LOG_DEBUG("useUnicodePips: " + std::to_string(useUnicodePips));
}

//----------------------------------------------------------------

ConsoleManager::~ConsoleManager()
{
    prepareForShutdown();
}

//----------------------------------------------------------------

ConsoleManager&
ConsoleManager::instance()
{
    static ConsoleManager mgr;
    return mgr;
}

//----------------------------------------------------------------

void
ConsoleManager::init()
{
    // TODO handle size of user window
    // int termRows, termCols;
    // getmaxyx(stdscr, termRows, termCols);
    // if (is_term_resized(41, 99)) {
    //     resize_term(41, 99);
    //    // Recalculate visible regions and redraw
    // }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    use_default_colors();

    inputThread_ = std::thread(&ConsoleManager::inputThreadFunc, this);
}

//----------------------------------------------------------------

void
ConsoleManager::prepareForShutdown()
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
ConsoleManager::draw(Surface* pSurface)
{
    pSurface->draw();
    doupdate();  // Paint the physical screen
}

//----------------------------------------------------------------

void
ConsoleManager::setSurface(Surface* pSurface)
{
    std::lock_guard<std::mutex> lock(stackMx_);
    for (auto* s : stack_) s->onDetach();
    stack_.clear();
    stack_.push_back(pSurface);
    pSurface->onAttach(nullptr);  // No parent to attach to.
    draw(pSurface);
}

//----------------------------------------------------------------

void
ConsoleManager::pushSurface(Surface* pSurface)
{
    Surface* pParent = nullptr;
    if (!stack_.empty())
    {
        pParent = stack_.back();
        pParent->onPause();
    }
    {
        std::lock_guard<std::mutex> lock(stackMx_);
        stack_.push_back(pSurface);
    }
    pSurface->onAttach(pParent);
    draw(pSurface);
}

//----------------------------------------------------------------

void
ConsoleManager::popSurface()
{
    {
        std::lock_guard<std::mutex> lock(stackMx_);
        if (stack_.empty()) return;

        auto* pSurface = stack_.back();
        stack_.pop_back();
        pSurface->onDetach();

        if (!stack_.empty())
        {
            stack_.back()->onResume();
        }
    }
    draw(stack_.back());
}

//----------------------------------------------------------------

void
ConsoleManager::popSurfaces()
{
    while (stack_.size() > 1)
    {
        popSurface();
        auto* pSurface = stack_.back();
        if (!pSurface->shouldSkip()) break;
    }
}

//----------------------------------------------------------------

void
ConsoleManager::inputThreadFunc()
{
    nodelay(stdscr, TRUE);
    while (running_)
    {
//      LOG_TRACE("ConsoleManager::inputThreadFunc() before wgetch()");
        int ch = wgetch(stdscr);
//      LOG_TRACE("ConsoleManager::inputThreadFunc() after wgetch(" + std::to_string(ch) + ")");
        if (ch == ERR)
        {
//          LOG_TRACE("ConsoleManager::inputThreadFunc() sleeping");
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        std::lock_guard<std::mutex> lk(stackMx_);
        if (!stack_.empty())
        {
            LOG_TRACE("ConsoleManager::inputThreadFunc() calling handlekey(" + std::to_string(ch) + ")");
            stack_.back()->handleKey(ch);
        }
    }
}

//----------------------------------------------------------------

bool
ConsoleManager::utf8_enabled()
{
    const char* loc = setlocale(LC_CTYPE, nullptr);
    if (!loc) return false;
    std::string s(loc);
    return s.find("UTF-8") != std::string::npos || s.find("utf8") != std::string::npos;
}

//----------------------------------------------------------------










//----------------------------------------------------------------

Consider later all these "show" functions when event handling
is re-architected. OK to stay here for now.

//----------------------------------------------------------------

void
ConsoleManager::showSuccess(
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
ConsoleManager::showErrorDialog(Ctrl::EventType    orgEventType,
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
ConsoleManager::showMakeBetSuccess(
    Craps::BetId betId,
    uint64_t correlationId)
{
    // TODO
}

//----------------------------------------------------------------

void
ConsoleManager::showMakeOddsBetSuccess(
    Craps::BetId betId,
    uint64_t correlationId)
{
    // TODO
}

//----------------------------------------------------------------

void
ConsoleManager::showMakeBetAutoSuccess(
    Craps::BetId betId,
    uint64_t correlationId)
{
    // TODO
}

//----------------------------------------------------------------

void
ConsoleManager::showMakeBetAutoError(
    Ctrl::EventType orgEventType,
    uint64_t correlationId,
    const Craps::PlayerId& playerId,
    const std::string& diag)
{
    // TODO
}

//----------------------------------------------------------------

void
ConsoleManager::showRollDiceCountDown(
    uint64_t correlationId,
    int      numSeconds)
{
    // TODO
}

//----------------------------------------------------------------

void
ConsoleManager::showRollDiceAnimation(uint64_t correlationId)
{
    // TODO
}

//----------------------------------------------------------------

void
ConsoleManager::showProgramExit()
{
    // TODO
}

//----------------------------------------------------------------
