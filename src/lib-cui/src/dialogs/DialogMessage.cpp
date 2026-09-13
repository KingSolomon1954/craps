//----------------------------------------------------------------
//
// File: DialogMessage.cpp
//
//----------------------------------------------------------------

#include <cui/dialogs/DialogMessage.h>
#include <cui/SurfaceManager.h>
#include <cui/CuiUtils.h>
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
    static DialogMessage dialog;
    return dialog;
}

//----------------------------------------------------------------
//
void
DialogMessage::configure(const MessageText& msg)
{
    assert(msg.type != MessageType::Unset);  // Must have a specified type
    eraseFields();
    msg_ = msg;
}

//----------------------------------------------------------------

void
DialogMessage::eraseFields()
{
    msg_.type = MessageType::Unset;
    msg_.customType.clear();
    msg_.brief.clear();
    msg_.detail.clear();
}

//----------------------------------------------------------------

void
DialogMessage::buildDialogEntries()
{
    formatTitle();
    formatDetail();
    formatPrompt();
}

//----------------------------------------------------------------

void
DialogMessage::formatTitle()
{
    // TODO add color / bold whatever
    if (msg_.type == MessageType::None)
    {
        msgFmtd_.title = msg_.brief;
    }
    if (msg_.type == MessageType::Custom)
    {
        msgFmtd_.title = msg_.customType;
        msgFmtd_.title += ":" +  msg_.brief;
    }
    if (msg_.type == MessageType::Info)
    {
        msgFmtd_.title = "Info: " + msg_.brief;
    }
    if (msg_.type == MessageType::Warning)
    {
        msgFmtd_.title = "Warning: " + msg_.brief;
    }
    if (msg_.type == MessageType::Warning)
    {
        msgFmtd_.title = "Warning: " + msg_.brief;
    }
    if (msg_.type == MessageType::Error)
    {
        msgFmtd_.title = "Error: " + msg_.brief;
    }
}

//----------------------------------------------------------------

void
DialogMessage::formatDetail()
{
    // TODO
    // Format a possibly large msg_.detail string into an array of 
    // strings. Each string is one line in a paragraph that is 
    // sized appropriately for the overall size of the detail and 
    // the width of the game screen.
    msgFmtd_.detail.clear();

    // TODO
}

//----------------------------------------------------------------

void
DialogMessage::formatPrompt()
{
    msgFmtd_.prompt = "[. or esc or enter] Back";
}

//----------------------------------------------------------------

void
DialogMessage::onAttach(SurfaceBase* pParent)
{
    assert(msg_.type != MessageType::Unset);  // Must first call configure() 
    
    SurfaceBase::onAttach(pParent);
    buildDialogEntries();
    calcSize();           // Determine our height/width
                          // winSize_ is now populated
    // Next, surface manager will ask for our locationRequest
}

//----------------------------------------------------------------

void
DialogMessage::onDetach()
{
    msg_.type = MessageType::Unset;  // Compell fresh configuration
}

//----------------------------------------------------------------

void
DialogMessage::calcSize()
{
    constexpr int titleRows  = 3; // Top border + title area
    constexpr int promptRows = 3; // Bottom border + prompt area
    constexpr int borderCols = 2; // 2 vertical borders
    constexpr int blankCols  = 2; // 2 empty columns adjacent to borders

    int cols = 0;
    for (const auto& e : msgFmtd_.detail)
    {
        cols = std::max(cols, static_cast<int>(e.length()));
    }
    winSize_.cols = cols + blankCols + borderCols;
    winSize_.rows = msgFmtd_.detail.size() + titleRows + promptRows;
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
    int row = 3;
    for (const auto& line : msgFmtd_.detail)
    {
        mvwprintw(pWin_, row++, 2, "%s", line.c_str());
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
    case '.':
        processSelection();
        return true;
        break;
        
    default:
        return false;
    }
}

//----------------------------------------------------------------

void
DialogMessage::processSelection()
{
    setOperationResult(OperationResult::Acknowledged);
    SurfaceManager::instance().popSurface();
}

//----------------------------------------------------------------














