//----------------------------------------------------------------
//
// File: DialogConfirm.cpp
//
//----------------------------------------------------------------

#include <cui/dialogs/DialogConfirm.h>
#include <cui/SurfaceManager.h>
#include <cui/CuiUtils.h>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

DialogConfirm::DialogConfirm()
    : DialogBase("DialogConfirm")
{
    // Create an initial WINDOW at location 0,0. Gets resized later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
}

//----------------------------------------------------------------

DialogConfirm&
DialogConfirm::instance()
{
    static DialogConfirm dialog;
    return dialog;
}

//----------------------------------------------------------------
//
void
DialogConfirm::configure(const std::string& textLine)
{
    text_ = textLine;
    configured_ = true;
}

//----------------------------------------------------------------

void
DialogConfirm::onAttach(SurfaceBase* pParent)
{
    assert(configured_);  // Must first call configure() 

    SurfaceBase::onAttach(pParent);
    setOperationResult(OperationResult::Unset);
    calcSize();           // Determine our height/width
                          // winSize_ is now populated
    // Next, surface manager will ask for our locationRequest
}

//----------------------------------------------------------------

void
DialogConfirm::onDetach()
{
    SurfaceBase::onDetach();
    configured_ = false;          // Detect fresh configuration
}

//----------------------------------------------------------------

void
DialogConfirm::calcSize()
{
    constexpr int titleRows  = 3; // Top border + title area
    constexpr int promptRows = 4; // Bottom border + prompt area
    constexpr int borderCols = 2; // 2 vertical borders
    constexpr int blankCols  = 2; // 2 empty columns adjacent to borders
    
    winSize_.cols = text_.length() + blankCols + borderCols;
    winSize_.rows = titleRows + promptRows;
}

//----------------------------------------------------------------
//
// SurfaceManager wants our window size and more.
// This occurs in context of SurfaceManager::pushSurface()
// SurfaceManager informs us shortly of our screen position.
// See setLocation() below. 
//
LocationRequest
DialogConfirm::getLocationRequest() const
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
DialogConfirm::setLocation(WindowPosition pos)
{
    winPos_ = pos;
    resize(winSize_);  // Dynamic menu needs resizing
    repos(winPos_);    // and repositioning
}

//----------------------------------------------------------------
//
// 0   ┌──────────────────────────┐
// 1   │ Confirm Quit RoyalCraps  │
// 2   ├──────────────────────────┤
// 3   │ [Y/y] Yes                │
// 4   │ [N/n] No                 │
// 5   │ [. or esc] Back          │
// 6   └──────────────────────────┘
//
void
DialogConfirm::draw()
{
    werase(pWin_);

    drawBorders();
    drawStaticContent();
    populate();
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------

void
DialogConfirm::drawBorders()
{
    box(pWin_, 0, 0);

    // Horizontal separator below the title.
    mvwhline(pWin_, 2, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, winSize_.cols - 1, ACS_RTEE);
}

//----------------------------------------------------------------

void
DialogConfirm::drawStaticContent()
{
    mvwprintw(pWin_, 3, 2, "[Y/y] yes");
    mvwprintw(pWin_, 4, 2, "[N/n] no");
    mvwprintw(pWin_, 5, 2, "[. or esc] Back");
}

//----------------------------------------------------------------

void
DialogConfirm::populate()
{
    mvwprintw(pWin_, 1, 2, "%s", text_.c_str());
}

//----------------------------------------------------------------

bool
DialogConfirm::handleKey(int ch)
{
    switch (ch)
    {
    case 'Y':
    case 'y':
        processSelection(OperationResult::Yes);
        return true;
        break;

    case 'N':
    case 'n':
        processSelection(OperationResult::No);
        return true;
        break;

    case '.':
    case  27:
        processSelection(OperationResult::Cancel);
        return true;
        break;

    default:
        return false;
    }
}

//----------------------------------------------------------------

void
DialogConfirm::processSelection(OperationResult r)
{
    setOperationResult(r);
    SurfaceManager::instance().popSurface();
}

//----------------------------------------------------------------














