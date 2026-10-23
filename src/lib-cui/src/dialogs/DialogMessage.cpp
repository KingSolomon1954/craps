//----------------------------------------------------------------
//
// File: DialogMessage.cpp
//
//----------------------------------------------------------------

#include <cui/dialogs/DialogMessage.h>
#include <cui/layouts/LayoutConsole.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/managers/ColorManager.h>
#include <cui/core/CuiUtils.h>
#include <gen/Logger.h>
#include <gen/StringUtils.h>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

DialogMessage::DialogMessage()
    : DialogBase("DialogMessage")
{
    // Create an initial WINDOW at location 0,0. Gets resized later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
}

//----------------------------------------------------------------

DialogMessage&
DialogMessage::instance()
{
    static DialogMessage dm;
    return dm;
}

/*-----------------------------------------------------------*//**

Caller provides what is to be displayed.

The formatting of the title area is:

    Severity introducer + ": " + brief

@li Introducer is determined by the MessageLabel::type field.
@li Uses precanned introducers for Info, Warning, and Error.
@li Uses custom introducer if MessageLabel::type == Custom.
@li No introducer at all if MessageLabel::type == None (just brief shows)

The formatting of the detail field adheres to these rules:

@li \\n = explicit paragraph/line break.
@li \\r\\n = Windows newline; normalize it.
@li \\r by itself = newline.
@li Multiple consecutive newlines are preserved as blank lines.
@li Normal whitespace separates words.
@li Lines are word-wrapped to a maximum width.
@li A word longer than the available width is hard-wrapped rather than
    overflowing the window.
@li Leading/trailing whitespace is discarded from each resulting display line.
*/
void
DialogMessage::configure(const MessageText& msg)
{
    assert(msg.label != MessageLabel::Unset);  // Must have a specified type
    eraseFields();
    msg_ = msg;
    
    int maxCols = buildDialogEntries();
    calcSize(maxCols);    // Determine our height/width
                          // winSize_ is now populated
}

//----------------------------------------------------------------

int
DialogMessage::buildDialogEntries()
{
    int maxCols = maxColsTitle();
    maxCols = std::max(maxCols, formatPrompt());
    formatText();
    maxCols = std::max(maxCols, maxColsDetail());
    return maxCols;
}

//----------------------------------------------------------------

void
DialogMessage::eraseFields()
{
    msg_.label = MessageLabel::Unset;
    msg_.customLabel.clear();
    msg_.brief.clear();
    msg_.detail.clear();
}

//----------------------------------------------------------------

void
DialogMessage::formatText()
{
    constexpr int preferredWidth = 76;
    constexpr int screenMargin   = 4;

    const int maxWidth = std::max(1, LayoutConsole::minCols - screenMargin);
    const int width    = std::min(preferredWidth, maxWidth);

    Gen::StringUtils::wrapFormattedString(msg_.detail, width, msgFmtd_.detail);
}

//----------------------------------------------------------------

int
DialogMessage::formatPrompt()
{
    msgFmtd_.prompt = "[. or esc or enter] Back";
    return msgFmtd_.prompt.size();
}

//----------------------------------------------------------------

int
DialogMessage::maxColsDetail()
{
    int cols = 0;
    for (const auto& e : msgFmtd_.detail)
    {
        cols = std::max(cols, static_cast<int>(e.length()));
    }
    return cols;
}

//----------------------------------------------------------------
//
// Figure out size of title. But we don't construct the title string
// here since we need to colorize parts of it later.
//
int
DialogMessage::maxColsTitle()
{
    int nCols = 0;
    
    if (msg_.label == MessageLabel::None)
        nCols = 0; // no introducer label, just the .brief
    else if (msg_.label == MessageLabel::Custom)
        nCols = msg_.customLabel.size();
    else if (msg_.label == MessageLabel::Info)
        nCols = InfoLabel_.size();
    else if (msg_.label == MessageLabel::Warning)
        nCols = WarnLabel_.size();
    else if (msg_.label == MessageLabel::Error)
        nCols = ErrorLabel_.size();
    else assert(false && "Unhandled label");
    
    return nCols + msg_.brief.size();
}

//----------------------------------------------------------------

void
DialogMessage::onAttach(SurfaceBase* pParent)
{
    assert(msg_.label != MessageLabel::Unset);  // Must first call configure() 
    
    SurfaceBase::onAttach(pParent);
    
    // Next, surface manager will ask for our locationRequest
}

//----------------------------------------------------------------

void
DialogMessage::onDetach()
{
    SurfaceBase::onDetach();
    msg_.label = MessageLabel::Unset;  // Detect fresh configuration later
}

//----------------------------------------------------------------
//
// Determine winSize_
//
void
DialogMessage::calcSize(int maxCols)
{
    constexpr int titleRows  = 3; // Top border + title area
    constexpr int promptRows = 3; // Bottom border + prompt area
    constexpr int borderCols = 2; // 2 vertical borders
    constexpr int blankCols  = 2; // 2 empty columns adjacent to borders

    winSize_.cols = maxCols + blankCols + borderCols;
    winSize_.rows = static_cast<int>(msgFmtd_.detail.size()) +
                    titleRows + promptRows;

    LOG_TRACE("DialogMessage::calcSize():"
              " winSize_.rows = " + std::to_string(winSize_.rows) +
              " winSize_.cols = " + std::to_string(winSize_.cols));
}

//----------------------------------------------------------------
//
// SurfaceManager wants our window size and more.
// This occurs in context of SurfaceManager::pushSurface()
// SurfaceManager informs us shortly of our screen position.
// See setLocation() below. 
//
LocationRequest
DialogMessage::getLocationRequest() const
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
DialogMessage::setLocation(WindowPosition pos)
{
    winPos_ = pos;
    resize(winSize_);  // Dynamic menu needs resizing
    repos(winPos_);    // and repositioning
}

//----------------------------------------------------------------
//
//     0123456789 123456789 123456789 123456789 123456789 123456789 1234
// 0   ┌───────────────────────────────────────────────────────────────┐
// 1   │ Warning: Insufficient funds                                   │
// 2   ├───────────────────────────────────────────────────────────────┤
// 3   │ Unable to make $100 Place Bet on 6 due to insufficient funds. │
// 4   │ Amount currently available to bet is: $76.                    │
// 5   │ Goto Control->Configure and add more money to your bank or    │
// 6   │ reduce your bet.                                              │
// 7   ├───────────────────────────────────────────────────────────────┤
// 8   │ [. or esc or enter] Back                                      │
// 9   └───────────────────────────────────────────────────────────────┘
//
void
DialogMessage::draw()
{
    werase(pWin_);

    drawBorders();
    drawPrompt();    
    drawTitle();
    drawText();
    
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------

void
DialogMessage::drawBorders()
{
    box(pWin_, 0, 0);

    // Horizontal separator below the title.
    mvwhline(pWin_, 2, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, winSize_.cols - 1, ACS_RTEE);

    // Horizontal separator above the prompt line.
    mvwhline(pWin_, winSize_.rows - 3, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, winSize_.rows - 3, 0, ACS_LTEE);
    mvwaddch(pWin_, winSize_.rows - 3, winSize_.cols - 1, ACS_RTEE);
}

//----------------------------------------------------------------

void
DialogMessage::drawPrompt()
{
    mvwprintw(pWin_, winSize_.rows - 2, 2, msgFmtd_.prompt.c_str());
}

//----------------------------------------------------------------

void
DialogMessage::drawTitle()
{
    drawCustomLabel();
    drawInfoLabel();
    drawWarnLabel();
    drawErrorLabel();
    
    waddstr(pWin_, msg_.brief.c_str());
}

//----------------------------------------------------------------

void
DialogMessage::drawCustomLabel()
{
    if (msg_.label != MessageLabel::Custom) return;
    attrOn(true);
    mvwaddstr(pWin_, 1, 2, msg_.customLabel.c_str());
    attrOn(false);
}
    
//----------------------------------------------------------------

void
DialogMessage::drawInfoLabel()
{
    if (msg_.label != MessageLabel::Info) return;
    attrOn(true);
    mvwaddstr(pWin_, 1, 2, InfoLabel_.c_str());
    attrOn(false);
}

//----------------------------------------------------------------

void
DialogMessage::drawWarnLabel()
{
    if (msg_.label != MessageLabel::Warning) return;
    attrOn(true);
    mvwaddstr(pWin_, 1, 2, WarnLabel_.c_str());
    attrOn(false);
}
    
//----------------------------------------------------------------

void
DialogMessage::drawErrorLabel()
{
    if (msg_.label != MessageLabel::Error) return;
    attrOn(true);
    mvwaddstr(pWin_, 1, 2, ErrorLabel_.c_str());
    attrOn(false);
}
    
//----------------------------------------------------------------

void
DialogMessage::drawText()
{        
    int row = 3;
    for (const auto& line : msgFmtd_.detail)
    {
        mvwprintw(pWin_, row++, 2, "%s", line.c_str());
    }
}

//----------------------------------------------------------------

void
DialogMessage::attrOn(bool onOff)
{
    if (msg_.label == MessageLabel::Custom)
        attrCustom(onOff);
    else if (msg_.label == MessageLabel::Info)
        attrInfo(onOff);
    else if (msg_.label == MessageLabel::Warning)
        attrWarn(onOff);
    else if (msg_.label == MessageLabel::Error)
        attrError(onOff);
}

//----------------------------------------------------------------

void
DialogMessage::attrCustom(bool onOff)
{
    if (onOff)
        wattron(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogMessageCustom)));
    else
        wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogMessageCustom)));
}

//----------------------------------------------------------------

void
DialogMessage::attrInfo(bool onOff)
{
    if (onOff)
        wattron(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogMessageInfo)));
    else
        wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogMessageInfo)));
}

//----------------------------------------------------------------

void
DialogMessage::attrWarn(bool onOff)
{
    if (onOff)
        wattron(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogMessageWarn)));
    else
        wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogMessageWarn)));
}

//----------------------------------------------------------------

void
DialogMessage::attrError(bool onOff)
{
    if (onOff)
        wattron(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogMessageError)));
    else
        wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogMessageError)));
}

//----------------------------------------------------------------

bool
DialogMessage::handleKey(int ch)
{
    switch (ch)
    {
    case 10 :  // enter key
    case 27 :  // escape key
    case '.': back(); return true;
    default : return false;
    }
}

//----------------------------------------------------------------

void
DialogMessage::back()
{
    setOperationResult(OperationResult::Acknowledged);
    SurfaceManager::instance().popSurface();
}

//----------------------------------------------------------------














