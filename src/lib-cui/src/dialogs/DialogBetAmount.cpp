//----------------------------------------------------------------
//
// File: DialogBetAmount.cpp
//
//----------------------------------------------------------------

#include <cui/dialogs/DialogBetAmount.h>
#include <cui/core/CuiUtils.h>
#include <cui/layouts/LayoutConsole.h>
#include <cui/managers/CarrierBet.h>
#include <cui/managers/SurfaceManager.h>
#include <gen/Logger.h>
#include <ncurses.h>
#include <limits>
#include <stdexcept>

using namespace Cui;

//----------------------------------------------------------------

DialogBetAmount::DialogBetAmount()
    : DialogBase("DialogBetAmount")
{
    // Create an initial WINDOW at location 0,0. Gets resized later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
    drawStaticContent();
}

//----------------------------------------------------------------

DialogBetAmount&
DialogBetAmount::instance()
{
    static DialogBetAmount da;
    return da;
}

//----------------------------------------------------------------

void
DialogBetAmount::onAttach(SurfaceBase* pParent)
{
    prepLineInput();
    curs_set(1);
}

//----------------------------------------------------------------

void
DialogBetAmount::onDetach()
{
    curs_set(0);
}

//----------------------------------------------------------------
//
// SurfaceManager wants our window size and more.
// This occurs in context of SurfaceManager::pushSurface()
// SurfaceManager informs us shortly of our screen position.
// See setLocation() below. 
//
LocationRequest
DialogBetAmount::getLocationRequest() const
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
DialogBetAmount::setLocation(WindowPosition pos)
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
DialogBetAmount::draw()
{
    // Reuse already filled window over and over
    drawTitle();
    drawInput();
    
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------
//
// Only done once at init time. These fields are never overwritten
// and we don't call werase().
//
void
DialogBetAmount::drawStaticContent()
{
    drawBorders();
    drawPrompts();
}
    
//----------------------------------------------------------------

void
DialogBetAmount::drawBorders()
{
    box(pWin_, 0, 0);

    // Horizontal separator below the title
    mvwhline(pWin_, 2, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, winSize_.cols - 1, ACS_RTEE);
}

//----------------------------------------------------------------

void
DialogBetAmount::drawPrompts()
{
    int row = 4;
    for (const std::string_view s : Prompts)
    {
        mvwaddstr(pWin_, row++, 2, s.data());
    }
}

//----------------------------------------------------------------

void
DialogBetAmount::drawTitle()
{
    const std::string title = CarrierBet::instance().getTitleBetAmount();
    const std::string s = prepTitle(title);
    mvwaddstr(pWin_, 1, 2, s.c_str());
}

//----------------------------------------------------------------

std::string
DialogBetAmount::prepTitle(const std::string& title)
{
    constexpr std::size_t MaxColsTitle = 22;
    std::string result = title.substr(0, MaxColsTitle);  // Too big
    result.resize(MaxColsTitle, ' ');                    // Pad it
    return result;
}

//----------------------------------------------------------------
//
// Override menu base class
//
bool
DialogBetAmount::handleKey(int ch)
{
    switch (ch)
    {
    case '\r':
    case '\n':
    case KEY_ENTER: doEnter();      return true;
    case 'a' : doAutoFill();        return true;
    case 'q' : doQuickBet();        return true;
    case '?' : doHelp();            return true;
    case '.' :
    case 27  : back();              return true;
    default:
        if (lineInput_.handleKey(ch))
        {
            drawInput();
            return true;
        }
        return false;
    }
}

//----------------------------------------------------------------

void
DialogBetAmount::back()
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

void
DialogBetAmount::drawInput()
{
    // Erase the entry field area to avoid leftover characters
    mvwaddstr(pWin_, inputRow_, inputCol_, eraseInput_.c_str());

    // Print the current buffer
    mvwaddstr(pWin_, inputRow_, inputCol_, lineInput_.value().c_str());

    // Place the cursor at the correct position
    wmove(pWin_, inputRow_, inputCol_ + static_cast<int>(lineInput_.cursorPosition()));

    wnoutrefresh(pWin_);
    doupdate();
}

//----------------------------------------------------------------

void
DialogBetAmount::doEnter()
{
    if (lineInput_.value().empty())
    {
        beep();  // Reject empty input
        return;
    }

    try
    {
        long long val = std::stoll(lineInput_.value());
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
        processAmount();
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
DialogBetAmount::doAutoFill()
{
    CarrierBet::instance().setAmount(inputAmount_);
    CarrierBet::instance().setAutoFill();
}    

//----------------------------------------------------------------

void
DialogBetAmount::doQuickBet()
{
    CarrierBet::instance().setAmount(inputAmount_);
    CarrierBet::instance().setQuickBet();
}    

//----------------------------------------------------------------

void
DialogBetAmount::doHelp()
{
    // TODO
}    

//----------------------------------------------------------------

void
DialogBetAmount::prepLineInput()
{
    lineInput_.clear();
    inputAmount_ = 0;
    const auto preFill = CarrierBet::instance().getPreFillAmount();
    if (preFill > 0)
        lineInput_.setValue(std::to_string(preFill));
}

//----------------------------------------------------------------

void
DialogBetAmount::processAmount()
{
    CarrierBet::instance().setAmount(inputAmount_);
    // TODO
    // Involve OperationManager:: make the bet
    // Hang around, we'll get popped by SurfaceManager when
    // op manager completes
}

//----------------------------------------------------------------

