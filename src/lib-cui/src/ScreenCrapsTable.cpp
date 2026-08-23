//----------------------------------------------------------------
//
// File: ScreenCrapsTable.cpp
//
//----------------------------------------------------------------

#include <cui/ScreenCrapsTable.h>
#include <cui/LayoutCrapsScreen.h>
#include <cui/CuiUtils.h>
#include <controller/CrapsGame.h>
#include <controller/CrapsReaders.h>
#include <gen/ErrorPass.h>
#include <gen/ReturnCode.h>
#include <gen/Logger.h>
#include <cassert>

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
}

//----------------------------------------------------------------
//
// External entry point to draw screen.
//
void
ScreenCrapsTable::draw()  // Override
{
    drawCrapsScreen();
}

//----------------------------------------------------------------
//
// Move window contents to ncurses virtual backing store
//
void

ScreenCrapsTable::transfer();
{
    wnoutrefresh(stdscr);
}

//----------------------------------------------------------------
//
// Internal entry point to draw craps screen.
//
void
ScreenCrapsTable::drawCrapsScreen()
{
    werase(pWindow_);
    
    LayoutCrapsScreen::drawBorders();

    pHeader_      ->draw();
    pRollHistory_ ->draw();
    pPlayerArea_  ->draw();
    pMessages_    ->draw();
    pAnimation_   ->draw();
    pHouseBrief_  ->draw();
    pPlayerBrief_ ->draw();
    ConsoleManager::getNavWindow()->draw();

    transfer(); 
    doupdate(); // Paint the actual terminal screen
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
ScreenCrapsTable::populateRollHistory()
{
    werase(w_.rollHistory);
    mvwprintw(w_.rollHistory,  0, 0, "Roll history here");
    wnoutrefresh(w_.rollHistory);
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

void
ScreenCrapsTable::onAttach()
{
    LOG_TRACE("ScreenCrapsTable::onAttach()");
    view_.pushScreen(pMenuBetting_);

    
    drawCrapsScreen();
    drawBorders();
    header.drawLayout();
    rollHistory.drawLayout();
    playerArea.drawLayout();
    ...
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
