//----------------------------------------------------------------
//
// File: ScreenCrapsTable.cpp
//
//----------------------------------------------------------------

#include <cui/ScreenCrapsTable.h>

#include <cassert>
#include <cui/ConsoleView.h>
#include <cui/MenuBetting.h>
#include <cui/CuiUtils.h>
#include <controller/CrapsGame.h>
#include <controller/CrapsReaders.h>
#include <gen/ErrorPass.h>
#include <gen/ReturnCode.h>
#include <gen/Logger.h>

using namespace Cui;

#include <iostream>
#include <chrono>

//----------------------------------------------------------------

ScreenCrapsTable::ScreenCrapsTable()
{
    Gen::ErrorPass ep;
    LOG_TRACE("Entered ScreenCrapsTable::ctor()");
    
    auto rc = Ctrl::CrapsReaders::getUserPlayer(userPlayerId_, ep);
    if (rc == Gen::ReturnCode::Fail)
    {
        ep.prepend("ScreenCrapsTable::ScreenCrapsTable(): unable to init; ");
        throw std::runtime_error(ep.diag);
    }

    rc = Ctrl::CrapsReaders::getActiveCrapsTable(tableId_, ep);
    if (rc == Gen::ReturnCode::Fail)
    {
        ep.prepend("ScreenCrapsTable::ScreenCrapsTable(): unable to init; ");
        throw std::runtime_error(ep.diag);
    }

    // Obtain our root menu and give it a pointer back to us.
    pMenuBetting_ = MenuBetting::instance();
    pMenuBetting_->setRootMenu(true);
    pMenuBetting_->setOwningScreen(this);

    createContentWindows();
    rc = Ctrl::CrapsReaders::readTablePlayers(tableId_, playerIds_, ep);
    assert(playerIds_.size() > 0);
    LOG_TRACE("Leaving ScreenCrapsTable::ctor()");
}

//----------------------------------------------------------------

ScreenCrapsTable*
ScreenCrapsTable::instance()
{
    static ScreenCrapsTable screenCrapsTable;
    return &screenCrapsTable;
}

//----------------------------------------------------------------

ScreenCrapsTable::~ScreenCrapsTable()
{
    if (w_.header)      delwin(w_.header);
    if (w_.rollHistory) delwin(w_.rollHistory);
    if (w_.playerArea)  delwin(w_.playerArea);
    if (w_.message)     delwin(w_.message);
    if (w_.animation)   delwin(w_.animation);
    if (w_.houseBrief)  delwin(w_.houseBrief);
    if (w_.playerBrief) delwin(w_.playerBrief);
}

//----------------------------------------------------------------

void
ScreenCrapsTable::createContentWindows()
{
    using L = LayoutCrapsScreen;

    w_.header      = newContentWindow(L::headerWinTopRow,      L::headerWinBotRow,      L::headerWinLeftCol,      L::headerWinRightCol);
    w_.rollHistory = newContentWindow(L::rollHistWinTopRow,    L::rollHistWinBotRow,    L::rollHistWinLeftCol,    L::rollHistWinRightCol);
    w_.playerArea  = newContentWindow(L::playerAreaWinTopRow,  L::playerAreaWinBotRow,  L::playerAreaWinLeftCol,  L::playerAreaWinRightCol);
    w_.message     = newContentWindow(L::messageWinTopRow,     L::messageWinBotRow,     L::messageWinLeftCol,     L::messageWinRightCol);
    w_.animation   = newContentWindow(L::animationWinTopRow,   L::animationWinBotRow,   L::animationWinLeftCol,   L::animationWinRightCol);
    w_.houseBrief  = newContentWindow(L::houseBriefWinTopRow,  L::houseBriefWinBotRow,  L::houseBriefWinLeftCol,  L::houseBriefWinRightCol);
    w_.playerBrief = newContentWindow(L::playerBriefWinTopRow, L::playerBriefWinBotRow, L::playerBriefWinLeftCol, L::playerBriefWinRightCol);

    assert(w_.header      != nullptr);
    assert(w_.rollHistory != nullptr);
    assert(w_.playerArea  != nullptr);
    assert(w_.message     != nullptr);
    assert(w_.animation   != nullptr);
    assert(w_.houseBrief  != nullptr);
    assert(w_.playerBrief != nullptr);
}

//----------------------------------------------------------------
//
// External entry point to draw screen.
//
void
ScreenCrapsTable::draw()
{
    drawCrapsScreen();
}

//----------------------------------------------------------------
//
// Internal entry point to draw craps screen.
//
void
ScreenCrapsTable::drawCrapsScreen()
{
// std::cout << "Howie 48 ScreenCrapsTable::draw() \n";
// std::this_thread::sleep_for(std::chrono::seconds(2));

    drawBorders();

    populateHeader();
    populateRollHistory();
    populateTable();
    populateMessages();
    populateAnimation();
    populateHouseBrief();
    populatePlayerBrief();

    // ConsoleView::populateNavBar()

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
ScreenCrapsTable::drawBorders()
{
    using L = LayoutCrapsScreen;

    // Draw full screen outside borders. 4 lines.
    mvhline(L:screenBorderRowTop, screenBorderColLeft,  ACS_HLINE, L::screenWidth);
    mvhline(L:screenBorderRowBot, screenBorderColLeft,  ACS_HLINE, L::screenWidth);
    mvvline(L:screenBorderRowTop, screenBorderColLeft,  ACS_VLINE, L::screenHeight);
    mvvline(L:screenBorderRowTop, screenBorderColRight, ACS_VLINE, L::screenHeight);

    // Patch up outer four corners
    mvaddch(L:screenBorderRowTop, screenBorderColLeft,  ACS_ULCORNER);
    mvaddch(L:screenBorderRowTop, screenBorderColRight, ACS_URCORNER);
    mvaddch(L:screenBorderRowBot, screenBorderColLeft,  ACS_LLCORNER);
    mvaddch(L:screenBorderRowBot, screenBorderColRight, ACS_LRCORNER);
    
    // Internal Horizontal lines
    mvhline(L::rollHistBorderTopRow,    L::rollHistBorderLeftCol,    ACS_HLINE, L::rollHistBorderRightCol);
    mvhline(L::playerAreaBorderTopRow,  L::playerAreaBorderLeftCol,  ACS_HLINE, L::playerAreaBorderRightCol);
    mvhline(L::messageBorderTopRow,     L::messageBorderLeftCol,     ACS_HLINE, L::messageBorderRightCol);
    mvhline(L::navBarBorderTopRow,      L::navBarBorderLeftCol,      ACS_HLINE, L::navBarWinRightCol);
    mvhline(L::houseBriefBorderTopRow,  L::houseBriefBorderLeftCol,  ACS_HLINE, L::houseBriefBorderRightCol);
    mvhline(L::playerBriefBorderTopRow, L::playerBriefBorderLeftCol, ACS_HLINE, L::playerBriefBorderRightCol);

    // Vertical lines
    mvvline(L::animationBorderTopRow,   L::animationBorderLeftCol,   ACS_VLINE, L::animationBorderBotRow   - L::animationBorderTopRow   + 1);
    mvvline(L::houseBriefBorderTopRow,  L::houseBriefBorderLeftCol,  ACS_VLINE, L::houseBriefBorderBotRow  - L::houseBriefBorderTopRow  + 1);
    mvvline(L::playerBriefBorderTopRow, L::playerBriefBorderLeftCol, ACS_VLINE, L::playerBriefBorderBotRow - L::playerBriefBorderTopRow + 1);

    // Junctions on top border
    mvaddch(L::headerBorderTopRow, L::headerBorderRightCol,  ACS_TTEE);

    // Junctions on left border
    mvaddch(L::rollHistBorderTopRow,   L::rollHistBorderLeftCol,   ACS_LTEE);
    mvaddch(L::playerAreaBorderTopRow, L::playerAreaBorderLeftCol, ACS_LTEE);
    mvaddch(L::messageBorderTopRow,    L::messageBorderLeftCol,    ACS_LTEE);

    // Junctions on right border
    mvaddch(L::houseBriefBorderTopRow,  L::houseBriefBorderRightCol,  ACS_RTEE);
    mvaddch(L::playerBriefBorderTopRow, L::playerBriefBorderRightCol, ACS_RTEE);
    mvaddch(L::navBarBorderTopRow,      L::navBarBorderRightCol,      ACS_RTEE);

    // Junctions at column split
    mvaddch(L::rollHistBorderTopRow,    L::rollHistBorderRigthCol,   ACS_RTEE);
    mvaddch(L::playerAreaBorderTopRow,  L::playerAreaBorderRightCol, ACS_RTEE);
    mvaddch(L::messageBorderTopRow,     L::messageBorderRight,       ACS_PLUS);
    mvaddch(L::playerBriefBorderTopRow, L::playerBriefBorderLeftCol, ACS_LTEE);
    mvaddch(L::playerBriefBorderBotRow, L::playerBriefBorderLeftCol, ACS_BTEE);

    // At this point, the player area window surrounded by borders.
    
    if (playerArea_ == PlayerView::AllPlayers)
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
// Fix up connectors for outer border of all players view
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
// Restores player area connectors to a smooth square
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
// Fix up connectors for outer border of one player view
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
ScreenCrapsTable::populateHeader()
{
    LOG_TRACE("ScreenCrapsTable::populateHeader()");
    werase(w_.header);
    mvwprintw(w_.header,  0, 0, "Header text here");
    wnoutrefresh(w_.header);
}

//----------------------------------------------------------------

void
ScreenCrapsTable::populateHistory()
{
    werase(w_.rollHistory);
    mvwprintw(w_.rollHistory,  0, 0, "Roll history here");
    wnoutrefresh(w_.rollHistory);
}

//----------------------------------------------------------------

void
ScreenCrapsTable::populatePlayerArea()
{
    werase(w_.playerArea);
    mvwprintw(w_.playerArea,  0, 0, "Player Area View here");
    if (playerArea_ == PlayerArea::AllPlayers)
    {
        populateAllPlayers();
    }
    else
    {
        populateOnePlayer();
    }
    wnoutrefresh(w_.playerArea);
}

//----------------------------------------------------------------

void
ScreenCrapsTable::populateAllPlayers()
{
    // w_.playerArea
}

//----------------------------------------------------------------

void
ScreenCrapsTable::populateOnePlayer()
{
    auto playerId = playerIds_[onePlayerIndex_];
    // w_.playerArea
}


//----------------------------------------------------------------

void ScreenCrapsTable::populateMessages()
{
    werase(w_.message);
    mvwprintw(w_.message,  0, 0, "Message area here");
    wnoutrefresh(w_.message);
}

//----------------------------------------------------------------

void ScreenCrapsTable::populateAnimation()
{
    werase(w_.animation);
    mvwprintw(w_.animation,  0, 0, "Animation area here");
    wnoutrefresh(w_.animation);
}

//----------------------------------------------------------------

void ScreenCrapsTable::populateHouseBrief()
{
    werase(w_.houseBrief);
    mvwprintw(w_.houseBrief,  0, 0, "House brief here");
    wnoutrefresh(w_.houseBrief);
}

//----------------------------------------------------------------

void ScreenCrapsTable::populatePlayerBrief()
{
    werase(w_.playerBrief);
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
// Redrawing functions. Prefixed with onXxx called by Menu processing
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
ScreenCrapsTable::onPlayerLeft(const Craps::PlayerId& pid)
{
    // TODO
}

//----------------------------------------------------------------



//----------------------------------------------------------------
//
// TODO hook in to menu processing
// TODO This changes to support next and previous
//
void
ScreenCrapsTable::cyclePlayerView()
{
    if (playerArea_ == PlayerArea::AllPlayers)
    {
        // Switch to first player
        playerArea_ = PlayerArea::OnePlayer;
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
            playerArea_ = PlayerArea::AllPlayers;
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
