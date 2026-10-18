//----------------------------------------------------------------
//
// File: DialogAmount.cpp
//
//----------------------------------------------------------------

#include <cui/dialogs/DialogAmount.h>
#include <cui/core/CuiUtils.h>
#include <cui/layouts/LayoutConsole.h>
#include <cui/managers/CarrierBet.h>
#include <cui/managers/SurfaceManager.h>
#include <gen/Logger.h>
#include <ncurses.h>
#include <cassert>
#include <cctype>
#include <cstring>

using namespace Cui;

//----------------------------------------------------------------

DialogAmount::DialogAmount()
    : DialogBase("DialogAmount")
{
    // Create an initial WINDOW at location 0,0. Gets resized later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
    drawStaticContent();
}

//----------------------------------------------------------------

DialogAmount&
DialogAmount::instance()
{
    static DialogAmount da;
    return da;
}

/*-----------------------------------------------------------*//**

Caller configures what is to be displayed.

This is designed as a fixed size window, sized to handle the
largest expected title string provided by the caller for bets,
which is this: "Dont Come Odds on the 10". Any
string longer than 24 characters is capped to 24.


Upon return, ErrorText is formatted and ready to be drawn.

ErrorText structure knows the number of sections to be drawn. Each
section is already formatted (wrapped as necessary), max rows and cols
of each section are known. winPos_ and winSize_ have been updated.

*/
void
DialogAmount::configure(const Config& cfg)
{
    clear();
    
    title_          = prepTitle(cfg.title);
    betName_        = cfg.betName;
    pivot_          = cfg.pivot;
    preFillAmount_  = cfg.preFillAmount;

    assert(betName_ != BetName::Invalid);
    assert(title_.size() > 0);
}

//----------------------------------------------------------------

std::string
DialogAmount::prepTitle(const std::string& title)
{
    constexpr std::size_t MaxColsTitle = 22;
    std::string result = title.substr(0, MaxColsTitle);  // Too big
    result.resize(MaxColsTitle, ' ');

    return result;
}

//----------------------------------------------------------------
//
// SurfaceManager wants our window size and more.
// This occurs in context of SurfaceManager::pushSurface()
// SurfaceManager informs us shortly of our screen position.
// See setLocation() below. 
//
LocationRequest
DialogAmount::getLocationRequest() const
{
    LocationRequest req;
    req.kind      = LocationKind::Dialog;
    req.size.rows = winSize_.rows;
    req.size.cols = winSize_.cols;
    req.direction = Direction::Right;
    return req;
}

//----------------------------------------------------------------
//
// SurfaceManager tells us our location.
// This occurs in context of SurfaceManager::pushSurface().
// We now have enough information to create/resize our
// ncurses WINDOW. Following this, SurfaceManager will
// call draw() on us.
//
void
DialogAmount::setLocation(WindowPosition pos)
{
    winPos_ = pos;
    resize(winSize_);  // Dynamic menu needs resizing
    repos(winPos_);    // and repositioning
}

//----------------------------------------------------------------
//
//    0123456789 123456789 1234567
// 0  ┌──────────────────────────┐
// 1  │ Dont Come Odds on the 10 │
// 2  ├──────────────────────────┤
// 3  │                          │
// 4  │ Enter amount: 100        │
// 5  │                          │
// 6  │ [enter] Submit Bet       │
// 7  │ [s] Save as AutoFill     │
// 8  │ [q] Save as Quick Bet    │
// 9  │ [?] Help                 │
// 10 │ [. or esc] Back          │
// 11 └──────────────────────────┘
//
void
DialogAmount::draw()
{
    // Just reuse already filled window over and over
    drawTitle();
    // touchwin(pWin_);
    
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------
//
// Only done once at init time. These fields are never overwritten
// and we don't call werase().
//
void
DialogAmount::drawStaticContent()
{
    drawBorders();
    drawPrompts();
}
    
//----------------------------------------------------------------

void
DialogAmount::drawBorders()
{
    box(pWin_, 0, 0);

    // Horizontal separator below the title
    mvwhline(pWin_, 2, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, winSize_.cols - 1, ACS_RTEE);
}

//----------------------------------------------------------------

void
DialogAmount::drawPrompts()
{
    int row = 4;
    for (const std::string_view s : Prompts)
    {
        mvwaddstr(pWin_, row++, 2, s.data());
    }
}

//----------------------------------------------------------------

void
DialogAmount::drawTitle()
{
    std::string title = CarrierBet::instance().getTitleAmount();
    mvwaddstr(pWin_, 1, 2, title.c_str());
}

//----------------------------------------------------------------
//
// Override menu base class
//
bool
DialogAmount::handleKey(int ch)
{
    switch (ch)
    {
    case '\r': 
    case '\n': doEnter();           return true;
    case 'a' : doAutoFill();        return true;
    case 'q' : doQuickBet();        return true;
    case '.' :
    case 27  : back();               return true;
    default  : return handleLineInput(ch);
    }
}

//----------------------------------------------------------------

void
DialogAmount::back()
{
    // Set our own state in base class to reflect cancel.
    // Also informs parent surfaces of the state of operation.
    // In turn, parent menus can decide if they are skipped
    // when unwinding the menu stack.
    //
    setOperationResult(OperationResult::Cancel);  // base class
    SurfaceManager::instance().popSurfaces();
}

//----------------------------------------------------------------
//
// Gather then interpret command line input
//
bool
DialogAmount::handleLineInput(int ch)
{
    if (ch == KEY_BACKSPACE || ch == 127)  // Backspace
    { 
        if (cursorPos_ > 0)
        {
            lineBuffer_.erase(cursorPos_ - 1, 1);
            --cursorPos_;
            drawInput();
        }
    }
    else if (ch == KEY_DC)                 // Delete key
    {
        if (cursorPos_ < lineBuffer_.size())
        {
            lineBuffer_.erase(cursorPos_, 1);
            drawInput();
        }
    }
    else if (ch == KEY_LEFT)               // Move cursor left
    {
        if (cursorPos_ > 0)
        {
            --cursorPos_;
            drawInput();
        }
    }
    else if (ch == KEY_RIGHT)              // Move cursor right
    {
        if (cursorPos_ < lineBuffer_.size())
        {
            ++cursorPos_;
            drawInput();
        }
    }
    else if (std::isdigit(ch))             // Only allow digits
    { 
        lineBuffer_.insert(cursorPos_, 1, static_cast<char>(ch));
        ++cursorPos_;
        drawInput();
    }
    else
    {
        return false;  // Ignore other keys
    }
    return true;
}

//----------------------------------------------------------------

void
DialogAmount::drawInput()
{
    // Determine where the input field starts
    int inputRow = 4;
    int inputCol = 2 + static_cast<int>(strlen("Enter amount: "));
    size_t numBlanks = winSize_.cols - inputCol;
    std::string eraseInput(numBlanks, ' ');
        
    // Clear the entry field area to avoid leftover characters
    mvwaddstr(pWin_, inputRow, inputCol, eraseInput.c_str());

    // Print the current buffer
    mvwprintw(pWin_, inputRow, inputCol, "%s", lineBuffer_.c_str());

    // Place the cursor at the correct position
    wmove(pWin_, inputRow, inputCol + static_cast<int>(cursorPos_));

    wnoutrefresh(pWin_);
}

//----------------------------------------------------------------

void
DialogAmount::doEnter()
{
    if (lineBuffer_.empty())
    {
        beep();  // Reject empty input
        return;
    }

    try
    {
        long long val = std::stoll(lineBuffer_);
        if (val < 0)
        {
            beep();  // Reject negative values
            return;
        }

        // Clamp to Gen::Money range if needed
        if (val > std::numeric_limits<Gen::Money>::max())
        {
            beep();
            return;
        }

        inputAmount_ = static_cast<Gen::Money>(val);
    }
    catch (const std::invalid_argument&)
    {
        beep(); // Non-numeric input
    }
    catch (const std::out_of_range&)
    {
        beep(); // Too large for stoll
    }
}

//----------------------------------------------------------------

void
DialogAmount::doAutoFill()
{
    (void) inputAmount_;
}    

//----------------------------------------------------------------

void
DialogAmount::doQuickBet()
{
    (void) inputAmount_;
}    

//----------------------------------------------------------------

void
DialogAmount::clear()
{
    lineBuffer_.clear();
    preFillAmount_ = 0;
    inputAmount_ = 0;
    cursorPos_   = 0;
}    

//----------------------------------------------------------------

void
DialogAmount::processAmount()
{
    // TODO
    // Fill out the Carrier Bet
    // OperationManager:: make the bet
    // We get popped SurfaceManager stack when op manager completes
}

//----------------------------------------------------------------

