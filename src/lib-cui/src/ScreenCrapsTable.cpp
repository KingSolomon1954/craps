//----------------------------------------------------------------
//
// File: ScreenCrapsTable.cpp
//
//----------------------------------------------------------------

#include <cui/ScreenCrapsTable.h>

#include <cassert>
#include <cui/ConsoleView.h>
#include <cui/MenuBetting.h>
#include <controller/CrapsGame.h>
#include <controller/CrapsInterface.h>
#include <gen/ErrorPass.h>
#include <gen/ReturnCode.h>
#include <gen/Logger.h>

using namespace Cui;

#include <iostream>
#include <chrono>

//----------------------------------------------------------------

ScreenCrapsTable::ScreenCrapsTable(ConsoleView& view)
    : Screen::Screen(view)
{
    Gen::ErrorPass ep;
    LOG_TRACE("Entered ScreenCrapsTable::ctor()");
    
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

    // Obtain our root menu and give it a pointer back to us.
    pMenuBetting_ = view_.getScreen(ConsoleView::ScreenId::MenuBetting);
    auto* pMenu = dynamic_cast<MenuBetting*>(pMenuBetting_);
    pMenu->setRootMenu(true);
    pMenu->setOwningScreen(this);

    createSubwindows();
    rc = Ctrl::CrapsInterface::tablePlayers(tableId_, playerIds_, ep);
    assert(playerIds_.size() > 0);
    LOG_TRACE("Leaving ScreenCrapsTable::ctor()");
}

//----------------------------------------------------------------

ScreenCrapsTable::~ScreenCrapsTable()
{
    if (w_.header)      delwin(w_.header);
    if (w_.history)     delwin(w_.history);
    if (w_.table  )     delwin(w_.table);
    if (w_.message)     delwin(w_.message);
    if (w_.animation)   delwin(w_.animation);
    if (w_.houseBrief)  delwin(w_.houseBrief);
    if (w_.playerBrief) delwin(w_.playerBrief);
}

//----------------------------------------------------------------

void
ScreenCrapsTable::createSubwindows()
{
    // TODO handle size of user window
    // int termRows, termCols;
    // getmaxyx(stdscr, termRows, termCols);

    using L = Layout;

    int leftInnerW  = L::leftW  - 2;
    int rightInnerW = L::rightW - 2;

    // Left column
    w_.header      = makeSubwin(0,             L::rowHistory,  leftInnerW, Column::Left);
    w_.history     = makeSubwin(L::rowHistory, L::rowTable,    leftInnerW, Column::Left);
    w_.table       = makeSubwin(L::rowTable,   L::rowMessage,  leftInnerW, Column::Left);
    w_.message     = makeSubwin(L::rowMessage, L::totalRows-1, leftInnerW, Column::Left);
    
    // Right column
    w_.animation   = makeSubwin(0,             L::rowMessage,  rightInnerW, Column::Right);
    w_.houseBrief  = makeSubwin(L::rowMessage, L::rowPlayer,   rightInnerW, Column::Right);
    w_.playerBrief = makeSubwin(L::rowPlayer,  L::totalRows-1, rightInnerW, Column::Right);

    assert(w_.header      != nullptr);
    assert(w_.history     != nullptr);
    assert(w_.table       != nullptr);
    assert(w_.message     != nullptr);
    assert(w_.animation   != nullptr);
    assert(w_.houseBrief  != nullptr);
    assert(w_.playerBrief != nullptr);
}

//----------------------------------------------------------------

WINDOW*
ScreenCrapsTable::makeSubwin(
    int topDivider,
    int bottomDivider,
    int innerW,
    Column col)
{
    int startY = topDivider + 1;
    int height = bottomDivider - topDivider - 1;
    int startX = (col == Column::Left) ? 1 : Layout::leftW + 1;

    return newwin(height, innerW, startY, startX);
}

//----------------------------------------------------------------
//
// External entry point to draw screen.
//
void
ScreenCrapsTable::draw()
{
// std::cout << "Howie 48 ScreenCrapsTable::draw() \n";
// std::this_thread::sleep_for(std::chrono::seconds(2));

   drawFrame();
   
   drawHeader();
   drawHistory();
   drawTable();
   drawMessages();
   drawAnimation();
   drawHouseBrief();
   drawPlayerBrief();

   // doupdate();  called by ConsoleView

    // When time comes, use conditional test for pip drawing
    // auto pip = [&](int y, int x) {
    //     if (view_.useUnicodePips) {
    //         const wchar_t pipChar[] = L"●";
    //         mvwaddwstr(w, y, x, pipChar);
    //     } else {
    //         mvwaddch(w, y, x, '*'); // ASCII fallback
    //     }
    // };
}

//----------------------------------------------------------------

void
ScreenCrapsTable::drawFrame()
{
    using L = Layout;

    // Horizontal lines
    mvhline(0,               0, ACS_HLINE, L::totalCols);
    mvhline(L::rowHistory,   0, ACS_HLINE, L::leftW);
    mvhline(L::rowTable,     0, ACS_HLINE, L::leftW);
    mvhline(L::rowMessage,   0, ACS_HLINE, L::leftW);
    mvhline(L::totalRows-1,  0, ACS_HLINE, L::totalCols);

    mvhline(L::rowMessage, L::leftW, ACS_HLINE, L::rightW);
    mvhline(L::rowPlayer,  L::leftW, ACS_HLINE, L::rightW);

    // Vertical lines
    mvvline(0, 0,              ACS_VLINE, L::totalRows);
    mvvline(0, L::totalCols-1, ACS_VLINE, L::totalRows);
    mvvline(0, L::leftW,       ACS_VLINE, L::totalRows);

    // Outer corners
    mvaddch(0,              0,              ACS_ULCORNER);
    mvaddch(0,              L::totalCols-1, ACS_URCORNER);
    mvaddch(L::totalRows-1, 0,              ACS_LLCORNER);
    mvaddch(L::totalRows-1, L::totalCols-1, ACS_LRCORNER);
    
    // Stable Junctions at left border
    mvaddch(L::rowHistory, 0, ACS_LTEE);
    mvaddch(L::rowTable,   0, ACS_LTEE);
    mvaddch(L::rowMessage, 0, ACS_LTEE);

    // Stable Junctions at right border
    mvaddch(L::rowMessage, L::totalCols-1, ACS_RTEE);
    mvaddch(L::rowPlayer,  L::totalCols-1, ACS_RTEE);

    // Stable Junctions at column split
    mvaddch(0,             L::leftW, ACS_TTEE);
    mvaddch(L::rowHistory, L::leftW, ACS_RTEE);
    mvaddch(L::rowTable,   L::leftW, ACS_RTEE);
    mvaddch(L::rowMessage, L::leftW, ACS_PLUS);
    mvaddch(L::rowPlayer,  L::leftW, ACS_LTEE);
    mvaddch(L::totalRows-1,L::leftW, ACS_BTEE);

    // At this point, the table subwindow is a smooth boxed area
    
    if (tableView_ == TableView::AllPlayers)
    {
        eraseConnectorsOnePlayer();
        drawConnectorsAllPlayers();
    }
    else
    {
        eraseConnectorsAllPlayers();
        drawConnectorsOnePlayer();
    }
    wnoutrefresh(stdscr);
}

//----------------------------------------------------------------
//
// Fix up connectors for outer border of all players table view
//
void
ScreenCrapsTable::drawConnectorsAllPlayers()
{
    using L = Layout;
    using A = LayoutAllPlayer;
    
    // Has 4 connectors to apply
    mvaddch(L::rowTable,   A::col2, ACS_TTEE);
    mvaddch(L::rowTable,   A::col3, ACS_TTEE);
    mvaddch(L::rowMessage, A::col2, ACS_BTEE);
    mvaddch(L::rowMessage, A::col3, ACS_BTEE);
}

//----------------------------------------------------------------
//
// Restores table area connectors to a smooth square
//
void
ScreenCrapsTable::eraseConnectorsAllPlayers()
{
    using L = Layout;
    using A = LayoutAllPlayer;
    
    // Has 4 connectors to restore
    mvaddch(L::rowTable,   A::col2, ACS_HLINE);
    mvaddch(L::rowTable,   A::col3, ACS_HLINE);
    mvaddch(L::rowMessage, A::col2, ACS_HLINE);
    mvaddch(L::rowMessage, A::col3, ACS_HLINE);
}

//----------------------------------------------------------------
//
// Fix up connectors for outer border of one player table view
//
void
ScreenCrapsTable::drawConnectorsOnePlayer()
{
    using L = Layout;
    using O = LayoutOnePlayer;

    // Top border
    mvaddch(L::rowTable, O::col2, ACS_TTEE);
    mvaddch(L::rowTable, O::col3, ACS_TTEE);
    mvaddch(L::rowTable, O::col4, ACS_TTEE);
    mvaddch(L::rowTable, O::col5, ACS_TTEE);
    mvaddch(L::rowTable, O::col6, ACS_TTEE);

    // Bottom border
    mvaddch(L::rowMessage, O::lineBetSplitCol, ACS_BTEE);

    // Left border    
    mvaddch(O::rowField,    0, ACS_LTEE);
    mvaddch(O::rowCraps,    0, ACS_LTEE);
    mvaddch(O::rowLineBets, 0, ACS_LTEE);
    
    // Right border    
    mvaddch(O::rowField,    L::leftW, ACS_RTEE);
    mvaddch(O::rowCraps,    L::leftW, ACS_RTEE);
    mvaddch(O::rowLineBets, L::leftW, ACS_RTEE);
}

//----------------------------------------------------------------
//
// Restores table area connectors to a smooth square
//
void
ScreenCrapsTable::eraseConnectorsOnePlayer()
{
    using L = Layout;
    using O = LayoutOnePlayer;

    // Top border
    mvaddch(L::rowTable, O::col2, ACS_HLINE);
    mvaddch(L::rowTable, O::col3, ACS_HLINE);
    mvaddch(L::rowTable, O::col4, ACS_HLINE);
    mvaddch(L::rowTable, O::col5, ACS_HLINE);
    mvaddch(L::rowTable, O::col6, ACS_HLINE);

    // Bottom border
    mvaddch(L::rowMessage, O::lineBetSplitCol, ACS_HLINE);

    // Left border    
    mvaddch(O::rowField,    0, ACS_VLINE);
    mvaddch(O::rowCraps,    0, ACS_VLINE);
    mvaddch(O::rowLineBets, 0, ACS_VLINE);
    
    // Right border    
    mvaddch(O::rowField,    L::leftW, ACS_VLINE);
    mvaddch(O::rowCraps,    L::leftW, ACS_VLINE);
    mvaddch(O::rowLineBets, L::leftW, ACS_VLINE);
}

//----------------------------------------------------------------

void
ScreenCrapsTable::drawHeader()
{
    LOG_TRACE("ScreenCrapsTable::drawHeader()");
    mvwprintw(w_.header,  0, 0, "Header text here");
    wnoutrefresh(w_.header);
}

//----------------------------------------------------------------

void
ScreenCrapsTable::drawHistory()
{
    mvwprintw(w_.history,  0, 0, "Roll history here");
    wnoutrefresh(w_.history);
}

//----------------------------------------------------------------

void
ScreenCrapsTable::drawTable()
{
    mvwprintw(w_.table,  0, 0, "Table View here");
    if (tableView_ == TableView::AllPlayers)
    {
        drawTableAllPlayers();
    }
    else
    {
        drawTableOnePlayer();
    }
    wnoutrefresh(w_.table);
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

void ScreenCrapsTable::drawMessages()
{
    mvwprintw(w_.message,  0, 0, "Message area here");
    wnoutrefresh(w_.message);
}

//----------------------------------------------------------------

void ScreenCrapsTable::drawAnimation()
{
    mvwprintw(w_.animation,  0, 0, "Animation area here");
    wnoutrefresh(w_.animation);
}

//----------------------------------------------------------------

void ScreenCrapsTable::drawHouseBrief()
{
    mvwprintw(w_.houseBrief,  0, 0, "House brief here");
    wnoutrefresh(w_.houseBrief);
}

//----------------------------------------------------------------

void ScreenCrapsTable::drawPlayerBrief()
{
    mvwprintw(w_.playerBrief,  0, 0, "Player brief here");
    wnoutrefresh(w_.playerBrief);
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
// Menu processing calls these onXxx functions for redrawing
//
//----------------------------------------------------------------

void
ScreenCrapsTable::onBetPlaced(const Craps::PlayerId& pid, Craps::BetId bid)
{
    // TODO
}

//----------------------------------------------------------------

void
ScreenCrapsTable::onBetFailed(const Craps::PlayerId& pid,
                              const std::string& reason)
{
    // TODO
}

//----------------------------------------------------------------

void
ScreenCrapsTable::onPlayerJoined(const Craps::PlayerId& pid)
{
    // TODO
}

//----------------------------------------------------------------

void
ScreenCrapsTable::onPlayerLeft  (const Craps::PlayerId& pid)
{
    // TODO
}

//----------------------------------------------------------------



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
    LOG_TRACE("ScreenCrapsTable::onAttach()");
    view_.pushScreen(pMenuBetting_);
}

//----------------------------------------------------------------
//
// Input is handled by menu MenuBetting.
//
void
ScreenCrapsTable::handleKey(int ch)
{
    (void) ch;
    LOG_TRACE("ScreenCrapsTable::handleKey() should "
              "not get here:(" + std::to_string(ch) + ")");
}

//----------------------------------------------------------------





#if 0

//----------------------------------------------------------------
//
// Interpret single-char menu actions
//
void
ScreenCrapsTable::handleMenuInput(int ch)
{
    if (ch == ' ')
    {
        // TODO: push/toggle menu screen
        return;
    }
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
    else if (ch == 'q')
    {
        setupQuickBet();
    }
    else if (ch == 'a')
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
//   werase(w_.command);
//    mvwprintw(w_.command, 0, 0, "> %s", lineBuffer_.c_str());
//    wrefresh(w_.command);
}

//----------------------------------------------------------------

void
ScreenCrapsTable::menuInputBetting(int ch)
{
//    LOG_TRACE("ScreenCrapsTable::menuInputBetting()");
    if (ch == 'b')
    {
        // back to previous menu
        view_.popScreen();
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
        view_.popScreen();
    }
    if (ch == 'q')
    {
        Ctrl::CrapsGame::instance()->terminateApp();
    }
}

#endif
