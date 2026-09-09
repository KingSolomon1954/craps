//----------------------------------------------------------------
//
// File: DialogConfirm.cpp
//
//----------------------------------------------------------------

#include <cui/dialogs/DialogConfirm.h>
#include <cui/SurfaceManager.h>
#include <cui/CuiUtils.h>
#include <algorithm>

using namespace Cui;

//----------------------------------------------------------------

DialogConfirm::DialogConfirm()
    : DialogBase("DialogConfirm")
{
    // No createWindow() here, uses dynamic WINDOW*
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
}

//----------------------------------------------------------------

void
DialogConfirm::resizeWindow()
{
    constexpr int borderCols = 2; // 2 vertical borders
    constexpr int blankCols  = 2; // 2 empty columns adjacent to borders

    int width = std::max<std::size_t>(text_.length(), 15);
    width += blankCols + borderCols;
    int height = 7;

    if (pWin_ == nullptr)
    {
        newWindow(height, width, winBorderTopCol_, winBorderTopRow_);
    }
    else
    {
        wresize(pWin_, height, width);
        mvwin(pWin_, winBorderTopCol_, winBorderTopRow_);
    }
}

//----------------------------------------------------------------

void
DialogConfirm::onAttach(SurfaceBase* pParent)
{
    DialogBase::onAttach(pParent);
    setOperationResult(OperationResult::Unset);
}

//----------------------------------------------------------------
//
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
    resizeWindow();
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
    
    int height, width;
    getmaxyx(pWin_, height, width);

    // Horizontal separator below the title.
    mvwhline(pWin_, 2, 1, ACS_HLINE, width - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, width - 1, ACS_RTEE);
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














