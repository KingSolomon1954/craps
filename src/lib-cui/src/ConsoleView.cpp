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
#include <cui/ScreenFactory.h>
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
    curs_set(0);
    start_color();
    use_default_colors();

    setScreen(getScreen(ScreenId::ScreenCrapsTable));  

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
        registry_.clear();
    }
    endwin();
}

//----------------------------------------------------------------

Screen*
ConsoleView::getScreen(ScreenId id)
{
    auto it = registry_.find(id);
    if (it == registry_.end())
    {
        std::unique_ptr<Screen> s(ScreenFactory::createSrceen(id, *this));
        registry_[id] = std::move(s);
    }
    return registry_[id].get();
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

    // find bottom-most full screen
    auto it = std::find_if(stack_.begin(), stack_.end(),
          [](Screen* s){ return s->type() == Screen::ScreenType::Full; });

    if (it != stack_.end())
    {
        werase(stdscr);
        (*it)->draw(); // draw base full screen
        ++it;
    }

    // draw overlays in order
    for (; it != stack_.end(); ++it)
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

bool
ConsoleView::utf8_enabled()
{
    const char* loc = setlocale(LC_CTYPE, nullptr);
    if (!loc) return false;
    std::string s(loc);
    return s.find("UTF-8") != std::string::npos || s.find("utf8") != std::string::npos;
}

//----------------------------------------------------------------


