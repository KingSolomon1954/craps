//----------------------------------------------------------------
//
// File: ConsoleView.cpp
//
//----------------------------------------------------------------

#include <cui/ConsoleView.h>
#include <cassert>
#include <chrono>
#include <controller/Globals.h>
#include <controller/GameEvent.h>
#include <controller/GameController.h>
#include <cui/Screen.h>
#include <cui/ScreenCrapsTable.h>

using namespace Cui;
using namespace std::chrono_literals;

//----------------------------------------------------------------

ConsoleView::ConsoleView() = default;

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

    running_ = true;
    inputThread_ = std::thread(&ConsoleView::inputThreadFunc, this);

    registerScreen(ScreenId::ScreenCrapsTable, std::make_unique<ScreenCrapsTable>(*this));
    // registerScreen(ScreenId::Stats, std::make_unique<StatsScreen>(*this));

    setScreen(ScreenId::ScreenCrapsTable);  // First screen is CrapsTable
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

void ConsoleView::inputThreadFunc()
{
    nodelay(stdscr, TRUE);    
    while (running_)
    {
        int ch = wgetch(stdscr);
        if (ch == ERR)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        std::lock_guard<std::mutex> lk(stackMx_);
        if (auto* top = topUnlocked())
        {
            top->handleKey(ch);
        }
    }
}

//----------------------------------------------------------------







#if 0

//----------------------------------------------------------------

void
ConsoleView::setInputMode(InputMode mode)
{
    std::lock_guard<std::mutex> lk(stackMx_);
    inputMode_ = mode;

    if (mode == InputMode::Line)
    {
        werase(inputWin_);
        mvwprintw(inputWin_, 0, 0, "> ");
        wrefresh(inputWin_);
    }
    else
    {
        if (inputWin_)
        {
            werase(inputWin_);
            wrefresh(inputWin_);
        }
    }
}


void
ConsoleView::inputThreadFunc()
{
    nodelay(stdscr, TRUE);    // non-blocking input on global screen
    while (running_)
    {
        int ch = wgetch(stdscr);
        if (ch == ERR)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        std::lock_guard<std::mutex> lk(stackMx_);
        if (inputMode_ == InputMode::Menu)
        {
            handleMenuInput(ch);
        }
        else
        {
            handleLineInput(ch);
        }
    }
}

//----------------------------------------------------------------

void
ConsoleView::handleMenuInput(int ch)
{
    std::string input(1, static_cast<char>(ch));
    if (!stack_.empty()) stack_.back()->handleInput(input);
}

//----------------------------------------------------------------

void
ConsoleView::handleLineInput(int ch)
{
    if (ch == '\n')
    {
        if (!stack_.empty())
        {
            stack_.back()->handleInput(lineBuffer_);
        }
        lineBuffer_.clear();
        werase(inputWin_);
        mvwprintw(inputWin_, 0, 0, "> ");
        wrefresh(inputWin_);
    }
    else if (ch == KEY_BACKSPACE || ch == 127)
    {
        if (!lineBuffer_.empty())
            lineBuffer_.pop_back();
    }
    else if (isprint(ch))
    {
        lineBuffer_.push_back(static_cast<char>(ch));
    }

    werase(inputWin_);
    mvwprintw(inputWin_, 0, 0, "> %s", lineBuffer_.c_str());
    wrefresh(inputWin_);
}

//----------------------------------------------------------------

#endif
