//----------------------------------------------------------------
//
// File: ScreenCrapsTable.cpp
//
//----------------------------------------------------------------

#include <cui/ScreenCrapsTable.h>
#include <cui/ConsoleView.h>
#include <controller/CrapsGame.h>

using namespace Cui;

#include <iostream>
#include <chrono>

//----------------------------------------------------------------

ScreenCrapsTable::ScreenCrapsTable(ConsoleView& view)
    : Screen::Screen(view)
{
std::cout << "Howie 21 ScreenCrapsTable::ScreenCrapsTable()\n";
std::this_thread::sleep_for(std::chrono::seconds(2));

    w_ = createSubwindows();
}

//----------------------------------------------------------------

ScreenCrapsTable::Windows
ScreenCrapsTable::createSubwindows()
{
int termRows, termCols;
getmaxyx(stdscr, termRows, termCols);

    Windows w{};
    
    w.header  = newwin(1, termCols, termRows-3, 0);
    w.command = newwin(1, termCols, termRows-6, 0);
    return w;
}

//----------------------------------------------------------------

ScreenCrapsTable::~ScreenCrapsTable()
{
    if (w_.header)  delwin(w_.header);
    if (w_.command) delwin(w_.command);
}

//----------------------------------------------------------------

void
ScreenCrapsTable::draw()
{
std::cout << "Howie 48 ScreenCrapsTable::draw() \n";
std::this_thread::sleep_for(std::chrono::seconds(2));
    werase(w_.header);
    werase(w_.command);
    box(w_.header, 0, 0);
    box(w_.command, 0, 0);
    mvwprintw(w_.header, 0, 2, "[ header ]");
    mvwprintw(w_.command, 0, 2, "[ command ]");
    wnoutrefresh(w_.header);
    wnoutrefresh(w_.command);
}

//----------------------------------------------------------------

void
ScreenCrapsTable::onAttach()
{
std::cout << "Howie 48 ScreenCrapsTable::onAttach() \n";
std::this_thread::sleep_for(std::chrono::seconds(2));

    setLineInputMode();
}

//----------------------------------------------------------------
//
// Input processing
//
//----------------------------------------------------------------

void
ScreenCrapsTable::handleKey(int ch)
{
std::cout << "Howie 69 ScreenCrapsTable::handleInput()\n";
std::this_thread::sleep_for(std::chrono::seconds(2));

    if (inputMode_ == Screen::InputMode::Menu)
    {
        handleMenuInput(ch);
    } 
    else
    {
        handleLineInput(ch);
    }
}

//----------------------------------------------------------------
//
// Interpret single-char menu actions
//
void
ScreenCrapsTable::handleMenuInput(int ch)
{
    switch(activeMenu_)
    {
    case Menus::Betting:  menuInputBetting(ch); break;
    case Menus::Stats:    menuInputStats(ch);   break;
    }
}

//----------------------------------------------------------------
//
// Gather then interpret command line input
//
void
ScreenCrapsTable::handleLineInput(int ch)
{
    if (ch == '\n')
    {
        processLineBuffer();
        lineBuffer_.clear();
        redrawInputPrompt();
    } 
    else if (ch == KEY_BACKSPACE || ch == 127)
    {
        if (!lineBuffer_.empty())
        {
            lineBuffer_.pop_back();
        }
        redrawInputPrompt();
    }
    else if (isprint(ch))
    {
        lineBuffer_.push_back(static_cast<char>(ch));
        redrawInputPrompt();
    }
}

//----------------------------------------------------------------

void
ScreenCrapsTable::redrawInputPrompt()
{
    werase(w_.command);
    mvwprintw(w_.command, 0, 0, "> %s", lineBuffer_.c_str());
    wrefresh(w_.command);
}

//----------------------------------------------------------------

void
ScreenCrapsTable::setMenuInputMode()
{
    inputMode_ = Screen::InputMode::Menu;
}

//----------------------------------------------------------------

void
ScreenCrapsTable::setLineInputMode()
{
    inputMode_ = Screen::InputMode::Line;
}

//----------------------------------------------------------------

void
ScreenCrapsTable::menuInputBetting(int ch)
{
    if (ch == 'b')
    {
        // back to previous menu
        view_.pushScreen(ConsoleView::ScreenId::Stats);
    }
    if (ch == 'q')
    {
        Ctrl::CrapsGame::instance()->terminateApp();
    }
}

//----------------------------------------------------------------

void
ScreenCrapsTable::menuInputStats(int ch)
{
    if (ch == 'b')
    {
        // back to previous menu 
        view_.pushScreen(ConsoleView::ScreenId::Stats);
    }
    if (ch == 'q')
    {
        Ctrl::CrapsGame::instance()->terminateApp();
    }
}

//----------------------------------------------------------------

void
ScreenCrapsTable::processLineBuffer()
{
}

//----------------------------------------------------------------
