//----------------------------------------------------------------
//
// File: DialogAmountEntry.cpp
//
//----------------------------------------------------------------

#include <cui/DialogAmountEntry.h>
#include <cassert>
#include <cstring>
#include <cctype>
#include <cui/ConsoleView.h>

using namespace Cui;

//----------------------------------------------------------------

DialogAmountEntry::DialogAmountEntry(ConsoleView& view)
    : MenuBase(view, 1, 1)  // Placeholder, resized in setMessage()
{
    visible_ = true;
}

//----------------------------------------------------------------

DialogAmountEntry::~DialogAmountEntry()
{
}

//----------------------------------------------------------------

void
DialogAmountEntry::setTitle(const std::string& title)
{
    title_ = title;
}

//----------------------------------------------------------------

void
DialogAmountEntry::setFillAmount(Gen::Money fillAmount)
{
    fillAmount_ = fillAmount;
}

//----------------------------------------------------------------

void
DialogAmountEntry::registerAutoFillCallback(Callback cb)
{
    autoFillCb_ = cb;
}

//----------------------------------------------------------------

void
DialogAmountEntry::registerQuickBetCallback(Callback cb)
{
    quickBetCb_ = cb;
}

//----------------------------------------------------------------
//
// Draw the menu prompts
//
// Override menu base class.
//
// Looks something like this:
//
// +------------------------+
// |  Place Bet on 6        |
// |                        |
// |  [Enter] Confirm Bet   |
// |  [a] Save as AutoFill  |
// |  [q] Save as QuickBet  |
// |  [ESC] Cancel          |
// |                        |
// |  Enter amount: 100     |
// +------------------------+
//
void
DialogAmountEntry::drawMenu()
{
    assert(w_ && "DialogAmountEntry: WINDOW not initialized");

    // Compute screen size
    int screenRows, screenCols;
    getmaxyx(stdscr, screenRows, screenCols);

    // Fixed box width (up to 80 cols)
    int boxWidth = std::min(80, screenCols - 2);

    // Static content lines: title + blank + 4 menu entries + blank + input
    int boxHeight = 9; // adjust if more lines added

    // Center on screen
    int startY = (screenRows - boxHeight) / 2;
    int startX = (screenCols - boxWidth) / 2;

    // Recreate the window
    if (w_) delwin(w_);
    w_ = newwin(boxHeight, boxWidth, startY, startX);

    // Draw border
    box(w_, 0, 0);

    // Print title at row 1, left-aligned
    mvwprintw(w_, 1, 2, "%s", title_.c_str());

    // Leave a blank line after the title
    int row = 2;

    // Menu options
    mvwprintw(w_, ++row, 2, "[Enter] Confirm Bet");
    mvwprintw(w_, ++row, 2, "[a]     Save as AutoFill");
    mvwprintw(w_, ++row, 2, "[q]     Save as QuickBet");
    mvwprintw(w_, ++row, 2, "[ESC]   Cancel");

    // Leave another blank line
    row++;

    // Input prompt
    mvwprintw(w_, ++row, 2, "Enter amount: ");

    // Draw the current input buffer and cursor
    drawInput();

    // Final refresh
    wnoutrefresh(w_);
}

//----------------------------------------------------------------
//
// Override menu base class
//
void
DialogAmountEntry::handleMenuKey(int ch)
{
    switch (ch)
    {
    case '\r': 
    case '\n': doEnter();    break;
    case 'a' : doAutoFill(); break;
    case 'q' : doQuickBet(); break;
    default  : handleLineInput(ch); break;
    }
}

//----------------------------------------------------------------
//
// Gather then interpret command line input
//
void
DialogAmountEntry::handleLineInput(int ch)
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
    // Ignore other keys
}

//----------------------------------------------------------------

void
DialogAmountEntry::drawInput()
{
    if (!w_) return;

    // Determine where the input field starts
    int inputRow = getmaxy(w_) - 2;   // second-to-last row inside box
    int inputCol = 2 + static_cast<int>(strlen("Enter amount: "));

    // Clear the field area to avoid leftover characters
    int maxWidth = getmaxx(w_) - inputCol - 2;  // leave room for border
    for (int i = 0; i < maxWidth; ++i)
    {
        mvwaddch(w_, inputRow, inputCol + i, ' ');
    }

    // Print the current buffer
    mvwprintw(w_, inputRow, inputCol, "%s", lineBuffer_.c_str());

    // Place the cursor at the correct position
    wmove(w_, inputRow, inputCol + static_cast<int>(cursorPos_));

    wnoutrefresh(w_);
}

//----------------------------------------------------------------

void
DialogAmountEntry::doEnter()
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

        amount_ = static_cast<Gen::Money>(val);
        view_.popScreen();
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
DialogAmountEntry::doAutoFill()
{
    autoFillCb_(amount_);
}    

//----------------------------------------------------------------

void
DialogAmountEntry::doQuickBet()
{
    quickBetCb_(amount_);
}    

//----------------------------------------------------------------

DialogAmountEntry::Results
DialogAmountEntry::getResults() const
{
    Results results;
    results.canceled = isCanceled_;
    results.amount   = amount_;
    return results;
}

//----------------------------------------------------------------
//
// The user calls this first upon their usage.
// 
void
DialogAmountEntry::clearState()
{
    title_.clear();
    lineBuffer_.clear();
    fillAmount_ = 0;
    amount_     = 0;
    cursorPos_  = 0;
    autoFillCb_ = nullptr;
    quickBetCb_ = nullptr;
    isCanceled_ = false;  // base class
}    

//----------------------------------------------------------------
