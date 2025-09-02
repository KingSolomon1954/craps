//----------------------------------------------------------------
//
// File: ScreenCrapsTable.cpp
//
//----------------------------------------------------------------

#include <cui/ScreenCrapsTable.h>

#include <cassert>
#include <cui/ConsoleView.h>
#include <controller/CrapsGame.h>
#include <controller/CrapsInterface.h>
#include <gen/ErrorPass.h>
#include <gen/ReturnCode.h>

using namespace Cui;

#include <iostream>
#include <chrono>

//----------------------------------------------------------------

ScreenCrapsTable::ScreenCrapsTable(ConsoleView& view)
    : Screen::Screen(view)
{
    Gen::ErrorPass ep;
    
    auto rc = Ctrl::CrapsInterface::getUserPlayer(userPlayerId_, ep);
    if (rc == Gen::ReturnCode::Fail)
    {
        ep.prepend("ScreenCrapsTable::ScreenCrapsTable(): unable to init; ");
        throw std::runtime_error(ep.diag);
    }

    rc = Ctrl::CrapsInterface::getActiveCrapsTable(tableId_, ep);
    if (rc == Gen::ReturnCode::Fail)
    {
        ep.prepend("ScreenCrapsTable::ScreenCrapsTable(): unable to init; ");
        throw std::runtime_error(ep.diag);
    }
    
std::cout << "Howie 21 ScreenCrapsTable::ScreenCrapsTable()\n";
std::this_thread::sleep_for(std::chrono::seconds(2));

    w_ = createSubwindows();
    rc = Ctrl::CrapsInterface::tablePlayers(tableId_, playerIds_, ep);
    assert(playerIds_.size() > 0);
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

   drawHeader();
   drawHistory();
   drawTable();
// TODO ...

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
ScreenCrapsTable::drawHeader()
{
}

//----------------------------------------------------------------

void
ScreenCrapsTable::drawHistory()
{
}

//----------------------------------------------------------------

void
ScreenCrapsTable::drawTable()
{
    if (tableView_ == TableView::AllPlayers)
    {
        drawTableAllPlayers();
    }
    else
    {
        drawTableOnePlayer();
    }
}

//----------------------------------------------------------------

void
ScreenCrapsTable::drawTableAllPlayers()
{
    // w_.table
}

//----------------------------------------------------------------

void
ScreenCrapsTable::drawTableOnePlayer()
{
    auto playerId = playerIds_[onePlayerIndex_];
    // w_.table
}

//----------------------------------------------------------------

Craps::PlayerId
ScreenCrapsTable::getPlayerAt(size_t index)
{
    // TODO lookup
    return 0;
}

//----------------------------------------------------------------
//
// TODO hook in to menu processing
//
void
ScreenCrapsTable::cycleTableView()
{
    if (tableView_ == TableView::AllPlayers)
    {
        // Switch to first player
        tableView_ = TableView::OnePlayer;
        onePlayerIndex_ = 0;
    }
    else
    {
        // Already in OnePlayer mode
        if (onePlayerIndex_ + 1 < playerIds_.size())
        {
            ++onePlayerIndex_;
        }
        else
        {
            // After last player → back to allPlayers
            tableView_ = TableView::AllPlayers;
            onePlayerIndex_ = 0;
        }
    }
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
    else if (inputMode_ == Screen::InputMode::Amount)
    {
        handleAmountInput(ch);
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
        drawInputPrompt();
    } 
    else if (ch == KEY_BACKSPACE || ch == 127)
    {
        if (!lineBuffer_.empty())
        {
            lineBuffer_.pop_back();
        }
        drawInputPrompt();
    }
    else if (isprint(ch))
    {
        lineBuffer_.push_back(static_cast<char>(ch));
        drawInputPrompt();
    }
}

//----------------------------------------------------------------
//
// Gather then interpret command line input
//
void
ScreenCrapsTable::handleAmountInput(int ch)
{
    // TODO add left/right arrow keys and deletion under cursor
    if (ch == '\n')
    {
        processLineBuffer();
        lineBuffer_.clear();
        drawInputPrompt();
    }
    else if (ch = 'q')
    {
        setupQuickBet();
    }
    else if (ch = 'a')
    {
        setupAutoFill();
    }
    else if (ch == KEY_BACKSPACE || ch == 127)
    {
        if (!lineBuffer_.empty())
        {
            lineBuffer_.pop_back();
        }
        drawInputPrompt();
    }
    else if (isprint(ch))
    {
        lineBuffer_.push_back(static_cast<char>(ch));
        drawInputPrompt();
    }
}

//----------------------------------------------------------------

void
ScreenCrapsTable::drawInputPrompt()
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
ScreenCrapsTable::setupQuickBet()
{
}

//----------------------------------------------------------------

void
ScreenCrapsTable::setupAutoFill()
{
}

//----------------------------------------------------------------

void
ScreenCrapsTable::processAmountBuffer()
{
}


//----------------------------------------------------------------

void
ScreenCrapsTable::processLineBuffer()
{
}

//----------------------------------------------------------------
