//----------------------------------------------------------------
//
// File: Consoleview.cpp
//
//----------------------------------------------------------------

#include <cui/ConsoleView.h>
#include <iostream>
#include <cassert>
#include <chrono>
#include <controller/Globals.h>
#include <controller/GameEvent.h>
#include <controller/GameController.h>

using namespace Cui;
using namespace std::chrono_literals;

//----------------------------------------------------------------

ConsoleView::ConsoleView()
{
    // Empty
}

//----------------------------------------------------------------

ConsoleView::~ConsoleView()
{
    shutdown();
}

//----------------------------------------------------------------

void
ConsoleView::init()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    mainWin_   = newwin(rows - 3, cols, 0, 0);
    statusWin_ = newwin(1, cols, rows - 3, 0);
    inputWin_  = newwin(2, cols, rows - 2, 0);

    scrollok(mainWin_, TRUE);

    running_ = true;
    inputThread_ = std::thread(&ConsoleView::inputThreadFunc, this);
    draw();
}

//----------------------------------------------------------------

void
ConsoleView::shutdown()
{
    running_ = false;
    if (inputThread_.joinable()) inputThread_.join();
    if (mainWin_)   delwin(mainWin_);
    if (statusWin_) delwin(statusWin_);
    if (inputWin_)  delwin(inputWin_);

    endwin();
}

//----------------------------------------------------------------

void
ConsoleView::draw()
{
    curs_set(0);
    werase(mainWin_);
//  box(mainWin_, 0, 0);
    wrefresh(mainWin_);

    werase(statusWin_);
    mvwprintw(statusWin_, 0, 0, "Status: ready");
    wrefresh(statusWin_);

    werase(inputWin_);
    mvwprintw(inputWin_, 0, 0, "> ");
    wrefresh(inputWin_);
    curs_set(1);
}

//----------------------------------------------------------------

void
ConsoleView::drawStatus(const std::string& msg)
{
    werase(statusWin_);
    mvwprintw(statusWin_, 0, 0, "Status: %s", msg.c_str());
    wrefresh(statusWin_);
}

//----------------------------------------------------------------

void
ConsoleView::drawMenu(const std::string& title, const std::vector<std::string>& items)
{
    werase(mainWin_);
    box(mainWin_, 0, 0);
    mvwprintw(mainWin_, 0, 2, "[ %s ]", title.c_str());
    for (size_t i = 0; i < items.size(); ++i) {
        mvwprintw(mainWin_, 2 + i, 2, "%c) %s", 'a' + i, items[i].c_str());
    }
    wrefresh(mainWin_);
}

//----------------------------------------------------------------

void
ConsoleView::drawPrompt(const std::string& prompt)
{
    werase(inputWin_);
    mvwprintw(inputWin_, 0, 0, "%s", prompt.c_str());
    wrefresh(inputWin_);
}

//----------------------------------------------------------------

void
ConsoleView::setInputMode(InputMode mode)
{
    mode_ = mode;
    lineBuffer_.clear();
}

//----------------------------------------------------------------

void
ConsoleView::inputThreadFunc()
{
    assert(inputWin_ != nullptr);
    nodelay(inputWin_, TRUE);  // non-blocking
    while (running_)
    {
        int ch = wgetch(inputWin_);
        if (ch == ERR)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }
        if (mode_ == InputMode::Menu)
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
    auto ev = std::make_shared<Ctrl::UserInputCharEvent>();
    ev->input = static_cast<char>(ch);
    Gbl::pGameCtrl->enqueue(ev);
}

//----------------------------------------------------------------

void
ConsoleView::handleLineInput(int ch)
{
    if (ch == '\n')
    {
        auto ev = std::make_shared<Ctrl::UserInputLineEvent>();
        ev->input = lineBuffer_;
        Gbl::pGameCtrl->enqueue(ev);

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

    // redraw buffer
    werase(inputWin_);
    mvwprintw(inputWin_, 0, 0, "> %s", lineBuffer_.c_str());
    wrefresh(inputWin_);
}

//----------------------------------------------------------------
