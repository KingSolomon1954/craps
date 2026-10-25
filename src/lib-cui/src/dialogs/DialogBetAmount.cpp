//----------------------------------------------------------------
//
// File: DialogBetAmount.cpp
//
//----------------------------------------------------------------

#include <cui/dialogs/DialogBetAmount.h>
#include <cui/core/CuiUtils.h>
#include <cui/layouts/LayoutConsole.h>
#include <cui/managers/CarrierBet.h>
#include <cui/managers/OperationManager.h>
#include <cui/managers/SurfaceManager.h>
#include <controller/AutoFill.h>
#include <controller/LastFill.h>
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
//    0123456789 123456789 1234567890
// 0  ┌─────────────────────────────┐
// 1  │ DontCome bet Odds on the 10 │
// 2  ├─────────────────────────────┤
// 3  │ Pays 1:2 use multiple of 2  │
// 4  │                             │
// 5  │ Enter amount: 100           │
// 6  │                             │
// 7  │ [enter] Submit Bet          │
// 8  │ [s] Save as AutoFill        │
// 9  │ [q] Save as Quick Bet       │
// 10 │ [?] Help                    │
// 11 │ [. or esc] Back             │
// 12 └─────────────────────────────┘
//
void
DialogBetAmount::draw()
{
    // Reuse already filled window over and over
    drawTitle();
    drawHintMultiples();
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
DialogBetAmount::drawHintMultiples()
{
    const auto& cb = CarrierBet::instance();

    // Group points with identical payout rules.
    enum class PointGroup { FourTen, FiveNine, SixEight, Other };

    PointGroup pointGroup = PointGroup::Other;
    switch (cb.getPivot())
    {
    case 4:
    case 10: pointGroup = PointGroup::FourTen; break;
    case 5:
    case 9:  pointGroup = PointGroup::FiveNine; break;
    case 6:
    case 8:  pointGroup = PointGroup::SixEight; break;
    }

    const auto index      = static_cast<std::size_t>(pointGroup);
    std::string_view hint = defaultHints[index];
    const auto betName    = cb.getBetName();
    
    if (cb.isOddsBet())
    {
        if (betName == BetName::PassLine || betName == BetName::Come)
            hint = passCome[index];
        else if (betName == BetName::DontPass || betName == BetName::DontCome)
            hint = dontPassCome[index];
    }
    else if (betName == BetName::Place)
        hint = place[index];
    else if (betName == BetName::Buy)
        hint = buy[index];
    else if (betName == BetName::Lay)
        hint = lay[index];

    mvwaddstr(pWin_, 3, 2, hint.data());
}

//----------------------------------------------------------------

void
DialogBetAmount::drawPrompts()
{
    int row = 5;
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
    constexpr std::size_t MaxColsTitle = 27;
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

bool
DialogBetAmount::readInputAmount()
{
    const std::string& value = lineInput_.value();

    if (value.empty())
        return false;

    try
    {
        std::size_t parsed = 0;
        const long long val = std::stoll(value, &parsed);

        // Require the entire input to be valid.
        if (parsed != value.size() ||
            val < 0 ||
            val > std::numeric_limits<Gen::Money>::max())
        {
            return false;
        }

        inputAmount_ = static_cast<Gen::Money>(val);
        return true;
    }
    catch (const std::invalid_argument&)
    {
        return false;
    }
    catch (const std::out_of_range&)
    {
        return false;
    }
}

//----------------------------------------------------------------

void
DialogBetAmount::doEnter()
{
    if (!readInputAmount())
    {
        beep();
        return;
    }

    processAmount();
}

//----------------------------------------------------------------

void
DialogBetAmount::doAutoFill()
{
    if (!readInputAmount())
    {
        beep();
        return;
    }

    CarrierBet::instance().setAmount(inputAmount_);
    OperationManager::instance().autoFill();
}

//----------------------------------------------------------------

void
DialogBetAmount::doQuickBet()
{
    if (!readInputAmount())
    {
        beep();
        return;
    }

    CarrierBet::instance().setAmount(inputAmount_);
    OperationManager::instance().quickBet();
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
    inputAmount_ = getPrefillAmount();
    lineInput_.setValue(std::to_string(inputAmount_));
}

//----------------------------------------------------------------

Gen::Money
DialogBetAmount::getPrefillAmount() const
{
    auto fill = getAutoFill();
    if (fill == 0) fill = getLastFill();
    return fill;
}

//----------------------------------------------------------------

Gen::Money
DialogBetAmount::getAutoFill() const
{
    const auto& cb = CarrierBet::instance();

    Ctrl::AutoFill::AutoFillEntry afe
    {
        cb.getBetName(),
        cb.getPivot(),
        cb.isOddsBet(),
        0                  // AutoFill returns preFillAmount here
    };

    Gen::ErrorPass ep;
    (void) Ctrl::CrapsReaders::getAutoFill(afe, ep);
    
    return afe.amount;
}
    
//----------------------------------------------------------------

Gen::Money
DialogBetAmount::getLastFill() const
{
    const auto& cb = CarrierBet::instance();

    Ctrl::LastFill::LastFillEntry lfe
    {
        cb.getBetName(),
        cb.getPivot(),
        cb.isOddsBet(),
        0                  // LastFill returns preFillAmount here
    };

    Gen::ErrorPass ep;
    (void) Ctrl::CrapsReaders::getLastFill(lfe, ep);
    
    return lfe.amount;
}

//----------------------------------------------------------------

void
DialogBetAmount::processAmount()
{
    CarrierBet::instance().setAmount(inputAmount_);
    // Keep this dialog active until the asynchronous request responds.
    // OperationManager owns the rest of this workflow.
    (void) OperationManager::instance().makeBet();
}

//----------------------------------------------------------------

