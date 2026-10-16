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
@li A word longer than the available width is hard-wrapped rather than overflowing the window.
@li Leading/trailing whitespace is discarded from each resulting display line.
*/
void
DialogMessage::configure(const MessageText& msg)
{
    assert(msg.label != MessageLabel::Unset);  // Must have a specified type
    eraseFields();
    msg_ = msg;
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

int
DialogMessage::buildDialogEntries()
{
    int maxCols = titleSize();
    maxCols = std::max(maxCols, formatPrompt());
    formatDetail();
    maxCols = std::max(maxCols, maxSizeDetail());
    return maxCols;
}

//----------------------------------------------------------------

int
DialogMessage::maxSizeDetail()
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
DialogMessage::titleSize()
{
    int ncols = msg_.brief.size();

    if (msg_.label == MessageLabel::None)
    {
        // no action, doesn't get an introducer label, just the .brief
    }
    else if (msg_.label == MessageLabel::Custom)
    {
        ncols += msg_.customLabel.size();
    }
    else if (msg_.label == MessageLabel::Info)
    {
        ncols += InfoLabel_.size();
    }
    else if (msg_.label == MessageLabel::Warning)
    {
        ncols += WarnLabel_.size();
    }
    else if (msg_.label == MessageLabel::Error)
    {
        ncols += ErrorLabel_.size();
    }
    else
    {
        assert(false && "Unhandled label");
    }
    return ncols;
}

//----------------------------------------------------------------

void
DialogMessage::formatDetail()
{
    constexpr int preferredWidth = 76;
    constexpr int screenMargin   = 4;

    msgFmtd_.detail.clear();
    if (msg_.detail.empty()) return;

    // Determine the maximum useful line width.
    const int maxWidth = std::max(1, LayoutConsole::minCols - screenMargin);
    const int width    = std::min(preferredWidth, maxWidth);
    
    // Normalize CRLF/CR into '\n'.  A newline is treated as an
    // explicit line/paragraph break.
    std::string text;
    text.reserve(msg_.detail.size());

    for (size_t i = 0; i < msg_.detail.size(); ++i)
    {
        if (msg_.detail[i] == '\r')
        {
            if (i + 1 < msg_.detail.size() &&
                msg_.detail[i + 1] == '\n')
            {
                ++i;
            }

            text += '\n';
        }
        else
        {
            text += msg_.detail[i];
        }
    }

    // Process one explicit line at a time.  This preserves blank
    // lines supplied by the caller.
    size_t start = 0;

    while (start <= text.size())
    {
        const size_t end = text.find('\n', start);
        const size_t len = (end == std::string::npos)
                         ? text.size() - start
                         : end - start;

        std::string line = text.substr(start, len);

        // Strip leading/trailing whitespace from the supplied line.
        const auto first = line.find_first_not_of(" \t");
        const auto last  = line.find_last_not_of(" \t");

        if (first == std::string::npos)
        {
            // Explicit blank line.
            msgFmtd_.detail.emplace_back();
        }
        else
        {
            line = line.substr(first, last - first + 1);

            size_t pos = 0;

            while (pos < line.size())
            {
                // Skip whitespace before the next word.
                while (pos < line.size() &&
                       (line[pos] == ' ' || line[pos] == '\t'))
                {
                    ++pos;
                }

                if (pos >= line.size())
                    break;

                const size_t remaining = line.size() - pos;

                // If the remaining text fits, we're done with this
                // explicit input line.
                if (remaining <= static_cast<size_t>(width))
                {
                    msgFmtd_.detail.push_back(line.substr(pos));
                    break;
                }

                // Find the last whitespace that fits in the line.
                size_t breakPos = pos + width;

                while (breakPos > pos &&
                       line[breakPos] != ' ' &&
                       line[breakPos] != '\t')
                {
                    --breakPos;
                }

                if (breakPos == pos)
                {
                    // A single word is longer than the display width.
                    // Hard-wrap it rather than allowing it to overflow.
                    msgFmtd_.detail.push_back(
                        line.substr(pos, width));

                    pos += width;
                }
                else
                {
                    msgFmtd_.detail.push_back(
                        line.substr(pos, breakPos - pos));

                    pos = breakPos;

                    // Skip whitespace at the wrap point.
                    while (pos < line.size() &&
                           (line[pos] == ' ' || line[pos] == '\t'))
                    {
                        ++pos;
                    }
                }
            }
        }

        if (end == std::string::npos)
            break;

        start = end + 1;
    }
}

//----------------------------------------------------------------

int
DialogMessage::formatPrompt()
{
    msgFmtd_.prompt = "[. or esc or enter] Back";
    return msgFmtd_.prompt.size();
}

//----------------------------------------------------------------

void
DialogMessage::onAttach(SurfaceBase* pParent)
{
    assert(msg_.label != MessageLabel::Unset);  // Must first call configure() 
    
    SurfaceBase::onAttach(pParent);
    int maxCols = buildDialogEntries();
    calcSize(maxCols);    // Determine our height/width
                          // winSize_ is now populated
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
    LOG_TRACE("DialogMessage::calcSize(): winSize_.rows = " +
              std::to_string(winSize_.rows) +  " winSize_.cols = " +
              std::to_string(winSize_.cols));
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
    drawStaticContent();
    populate();
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
DialogMessage::drawStaticContent()
{
    mvwprintw(pWin_, winSize_.rows - 2, 2, msgFmtd_.prompt.c_str());
}

//----------------------------------------------------------------

void
DialogMessage::populate()
{
    drawTitle();
    drawDetail();
}

//----------------------------------------------------------------

void
DialogMessage::drawTitle()
{
    if (msg_.label == MessageLabel::None)
    {
        // do nothing
    }
    else if (msg_.label == MessageLabel::Custom)
    {
        attrOn();
        mvwaddstr(pWin_, 1, 2, msg_.customLabel.c_str());
        attrOff();
    }
    else if (msg_.label == MessageLabel::Info)
    {
        attrOn();
        mvwaddstr(pWin_, 1, 2, InfoLabel_.c_str());
        attrOff();
    }
    else if (msg_.label == MessageLabel::Warning)
    {
        attrOn();
        mvwaddstr(pWin_, 1, 2, WarnLabel_.c_str());
        attrOff();
    }
    else if (msg_.label == MessageLabel::Error)
    {
        attrOn();
        mvwaddstr(pWin_, 1, 2, ErrorLabel_.c_str());
        attrOff();
    }
    else
    {
        assert(false && "Unhandled title label");
    }
    waddstr(pWin_, msg_.brief.c_str());
}

//----------------------------------------------------------------

void
DialogMessage::drawDetail()
{        
    int row = 3;
    for (const auto& line : msgFmtd_.detail)
    {
        mvwprintw(pWin_, row++, 2, "%s", line.c_str());
    }
}

//----------------------------------------------------------------

void
DialogMessage::attrOn()
{
    if (msg_.label == MessageLabel::None)
    {
        // do nothing
    }
    else if (msg_.label == MessageLabel::Custom)
    {
        wattron(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::CustomMessage)));
    }
    else if (msg_.label == MessageLabel::Info)
    {
        wattron(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::InfoMessage)));
    }
    else if (msg_.label == MessageLabel::Warning)
    {
        wattron(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::WarnMessage)));
    }
    else if (msg_.label == MessageLabel::Error)
    {
        wattron(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::ErrorMessage)));
    }
}

//----------------------------------------------------------------

void
DialogMessage::attrOff()
{
    if (msg_.label == MessageLabel::None)
    {
        // do nothing
    }
    else if (msg_.label == MessageLabel::Custom)
    {
        wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::CustomMessage)));
    }
    else if (msg_.label == MessageLabel::Info)
    {
        wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::InfoMessage)));
    }
    else if (msg_.label == MessageLabel::Warning)
    {
        wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::WarnMessage)));
    }
    else if (msg_.label == MessageLabel::Error)
    {
        wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::ErrorMessage)));
    }
}

//----------------------------------------------------------------

bool
DialogMessage::handleKey(int ch)
{
    switch (ch)
    {
    case 10 :  // enter key
    case 27 :  // escape key
    case '.': back(); return true; break;
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














