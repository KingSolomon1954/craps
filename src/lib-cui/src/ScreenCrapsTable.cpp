//----------------------------------------------------------------
//
// File: ScreenCrapsTable.cpp
//
//----------------------------------------------------------------

#include <cui/ScreenCrapsTable.h>
#include <cui/LayoutCrapsScreen.h>
#include <gen/Logger.h>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

ScreenCrapsTable::ScreenCrapsTable()
{
    LOG_TRACE("Entered ScreenCrapsTable::ctor()");

    createWindow();

    // Obtain our root menu and give it a pointer back to us.
    pMenuBetting_ = MenuBetting::instance();
    pMenuBetting_->setRootMenu(true);
    pMenuBetting_->setOwningScreen(this);

    LOG_TRACE("Leaving ScreenCrapsTable::ctor()");
}

//----------------------------------------------------------------

void
ScreenCrapsTable::releaseNcursesResources()
{
    wHeader_.releaseNcursesResources();
    wRollHistory_.releaseNcursesResources();
    wPlayerArea_.releaseNcursesResources();
    wMessages_.releaseNcursesResources();
    wAnimation_.releaseNcursesResources();
    wHouseBrief_.releaseNcursesResources();
    wPlayerBrief_.releaseNcursesResources();
    
    Surface::releaseNcursesResources();  // Execute base implementation    
}

//----------------------------------------------------------------

ScreenCrapsTable&
ScreenCrapsTable::instance()
{
    static ScreenCrapsTable& screenCrapsTable;
    return screenCrapsTable;
}

//----------------------------------------------------------------

void
ScreenCrapsTable::createWindow()
{
    using L = LayoutCrapsScreen;
    
    pWin_ = newwin(L::height, L::width, L::leftCol, L::topRow);
}

//----------------------------------------------------------------
//
// External entry point to draw screen.
// 
// Each window draw() below does its own transfer()
// Caller is responsible for eventual doupdate()
//
void
ScreenCrapsTable::draw()  // Override
{
    werase(pWin_);
    
    LayoutCrapsScreen::draw();

    wHeader_.draw();
    wRollHistory_.draw();
    wPlayerArea_.draw();
    wMessages_.draw();
    wAnimation_.draw();
    wHouseBrief_.draw();
    wPlayerBrief_.draw();
    drawNavBar();
}

//----------------------------------------------------------------
//
// ScreenCrapsTable does not process keys directly. MenuBetting owns
// and handles keyboard processing while this surface is active.
//
void
ScreenCrapsTable::handleKey(int ch)
{
    (void)ch;
    LOG_TRACE("ScreenCrapsTable::handleKey() should "
              "not get here:(" + std::to_string(ch) + ")");
}

//----------------------------------------------------------------

void
ScreenCrapsTable::drawNavBar()
{
    auto nav = WindowNavBar::instance();
    
    nav.clear();
    nav.configure(
    "[C] Control [B] BETTING [V] View [S] Stats | [H] Help [Q] Quit");
}

//----------------------------------------------------------------

WINDOW*
ScreenCrapsTable::lendWindow()
{
    return pWin_;
}

//----------------------------------------------------------------



#if 0

// Not currently used. Just here a while for reference.

//----------------------------------------------------------------
//
// What might be done in onAttach()
//
// * Start a timer/animation associated with the surface.
// * Subscribe to events that should only be received while
//   the surface is active.
// * Reset transient interaction state.
// * Establish the initial input mode.
// * Set the NavBar/context for the surface, if you decide to do
//   that there.
// * Trigger an initial data acquisition if appropriate.
//    
void
ScreenCrapsTable::onAttach(Surface* pParent)
{
    LOG_TRACE("ScreenCrapsTable::onAttach()");
    // TODO
    // Maybe get the user player. Maybe user changed via the
    // control menu. Should be listening for new user events.
}

//----------------------------------------------------------------
//
// Basically undo what was done during onAttach()
//
void
ScreenCrapsTable::onDetach()
{
    LOG_TRACE("ScreenCrapsTable::onDetach()");
    // TODO
}

//----------------------------------------------------------------
//
// Stop timers if any and so on.
//
void
ScreenCrapsTable::onPause()
{
    LOG_TRACE("ScreenCrapsTable::onPause()");
    // TODO
}

//----------------------------------------------------------------
//
// Start/restart timers if any and so on.
//
void
ScreenCrapsTable::onResume()
{
    LOG_TRACE("ScreenCrapsTable::onResume()");
    // TODO
}

#endif






#if 0

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

std::string lineBuffer_;

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

#include <controller/CrapsGame.h>

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
