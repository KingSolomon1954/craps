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

    registerScreen(ScreenId::ScreenCrapsTable, std::make_unique<ScreenCrapsTable>(*this));
    // registerScreen(ScreenId::Stats, std::make_unique<StatsScreen>(*this));

    setScreen(ScreenId::ScreenCrapsTable);  // First screen is CrapsTable

    running_ = true;
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
        // Clear registry and stack before ending ncurses
        std::lock_guard<std::mutex> lk(stackMx_);
        stack_.clear();
        registry_.clear();
    }
    endwin();
}

//----------------------------------------------------------------

void
ConsoleView::registerScreen(ScreenId id, std::unique_ptr<Screen> screen)
{
    registry_[id] = std::move(screen);
}

//----------------------------------------------------------------

void
ConsoleView::setScreen(ScreenId id)
{
    std::lock_guard<std::mutex> lk(stackMx_);
    auto it = registry_.find(id);
    if (it == registry_.end()) return;
    setScreenUnlocked(it->second.get());
}

//----------------------------------------------------------------

void
ConsoleView::pushScreen(ScreenId id)
{
    std::lock_guard<std::mutex> lk(stackMx_);
    auto it = registry_.find(id);
    if (it == registry_.end()) return;

    if (auto* top = topUnlocked()) top->onPause();
    auto* s = it->second.get();
    stack_.push_back(s);
    s->onAttach();
    redrawUnlocked();
}

//----------------------------------------------------------------

void
ConsoleView::popScreen()
{
    std::lock_guard<std::mutex> lk(stackMx_);
    if (stack_.empty()) return;

    Screen* oldTop = stack_.back();
    stack_.pop_back();
    oldTop->onDetach();

    if (auto* nowTop = topUnlocked())
    {
        nowTop->onResume();
    }
    redrawUnlocked();
}

//----------------------------------------------------------------

Screen*
ConsoleView::topUnlocked()
{
    return stack_.empty() ? nullptr : stack_.back();
}

//----------------------------------------------------------------

void
ConsoleView::redrawUnlocked()
{
    // Draw bottom->top using *wnoutrefresh* in each screen's draw()
    werase(stdscr);
    for (auto* s : stack_) s->draw();
    doupdate();
}

//----------------------------------------------------------------

void
ConsoleView::setScreenUnlocked(Screen* s)
{
    // detach all
    while (!stack_.empty())
    {
        stack_.back()->onDetach();
        stack_.pop_back();
    }
    stack_.push_back(s);
    s->onAttach();
    redrawUnlocked();
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
        if (auto* top = topUnlocked())
        {
            LOG_TRACE("ConsoleView::inputThreadFunc() calling handlekey(" + std::to_string(ch) + ")");
            top->handleKey(ch);
        }
    }
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


