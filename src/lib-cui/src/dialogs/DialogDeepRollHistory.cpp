//----------------------------------------------------------------
//
// File: DialogDeepRollHistory.cpp
//
//----------------------------------------------------------------

#include <cui/dialogs/DialogDeepRollHistory.h>
#include <cui/managers/ColorManager.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/core/CuiUtils.h>
#include <gen/Logger.h>
#include <cstdio>

using namespace Cui;

//----------------------------------------------------------------

DialogDeepRollHistory::DialogDeepRollHistory()
    : DialogBase("DialogDeepRollHistory")
{
    winSize_.rows = WinRows;
    winSize_.cols = WinCols;
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
}

//----------------------------------------------------------------

DialogDeepRollHistory&
DialogDeepRollHistory::instance()
{
    static DialogDeepRollHistory d;
    return d;
}

//----------------------------------------------------------------

void
DialogDeepRollHistory::show()
{
    SurfaceManager::instance().pushSurface(this);
}

//----------------------------------------------------------------

LocationRequest
DialogDeepRollHistory::getLocationRequest() const
{
    LocationRequest req;
    req.kind      = LocationKind::Dialog;
    req.size.rows = WinRows;
    req.size.cols = WinCols;
    req.direction = Direction::Right;
    return req;
}

//----------------------------------------------------------------

void
DialogDeepRollHistory::setLocation(WindowPosition pos)
{
    winPos_ = pos;
    repos(winPos_);        // Fixed size; only needs placing
}

//----------------------------------------------------------------
//
// Cell text, right justified in CellWidth columns:
//
//    7(3,4)
//   10(6,5)
//
std::string
DialogDeepRollHistory::cellText(const RollEntry& r)
{
    char buf[32];
    std::snprintf(buf, sizeof buf, "%u(%u,%u)", r.d1 + r.d2, r.d1, r.d2);
    std::string s = buf;
    if (static_cast<int>(s.size()) < CellWidth)
    {
        s.insert(0, CellWidth - s.size(), ' ');
    }
    return s;
}

//----------------------------------------------------------------
//
// Newest is index 0 at the top of column 0, then down, then column 1.
//
std::size_t
DialogDeepRollHistory::indexAt(int row, int col)
{
    return static_cast<std::size_t>(col * GridRows + row);
}

//----------------------------------------------------------------
//
//      0123456789 123456789 123456789 123456789 123456789 123456789 1
//  0   ┌───────────────────────────────────────────────────────────┐
//  1   │ Deep Roll History                                         │
//  2   ├─────────┬─────────┬─────────┬─────────┬─────────┬─────────┤
//  3   │  7(3,4) │  7(3,4) │ ...                                   │
//  .   │   ...                                                     │
// 22   │ 12(6,6) │  8(5,3) │ ...                                   │
// 23   ├─────────┴─────────┴─────────┴─────────┴─────────┴─────────┤
// 24   │ [. or esc] Back                                           │
// 25   └───────────────────────────────────────────────────────────┘
//
void
DialogDeepRollHistory::draw()
{
    werase(pWin_);

    drawBorders();
    mvwaddstr(pWin_, 1, 2, "Deep Roll History");
    mvwaddstr(pWin_, WinRows - 2, 2, "[. or esc] Back");
    drawGrid();

    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------

void
DialogDeepRollHistory::drawBorders()
{
    box(pWin_, 0, 0);

    const int topSep = 2;
    const int botSep = WinRows - 3;

    mvwhline(pWin_, topSep, 1, ACS_HLINE, WinCols - 2);
    mvwaddch(pWin_, topSep, 0, ACS_LTEE);
    mvwaddch(pWin_, topSep, WinCols - 1, ACS_RTEE);

    mvwhline(pWin_, botSep, 1, ACS_HLINE, WinCols - 2);
    mvwaddch(pWin_, botSep, 0, ACS_LTEE);
    mvwaddch(pWin_, botSep, WinCols - 1, ACS_RTEE);

    // Vertical lines between the columns, with tees on the separators
    for (int c = 1; c < GridCols; c++)
    {
        const int x = c * (CellSpan + 1);
        mvwvline(pWin_, topSep + 1, x, ACS_VLINE, GridRows);
        mvwaddch(pWin_, topSep, x, ACS_TTEE);
        mvwaddch(pWin_, botSep, x, ACS_BTEE);
    }
}

//----------------------------------------------------------------

void
DialogDeepRollHistory::drawGrid()
{
    const auto& rolls = RollLog::instance().rolls();

    if (rolls.empty())
    {
        mvwaddstr(pWin_, 3, 2, "No rolls yet");
        return;
    }

    auto& colors = ColorManager::instance();

    for (int col = 0; col < GridCols; col++)
    {
        const int x = 1 + col * (CellSpan + 1) + 1;   // after the line and a blank

        for (int row = 0; row < GridRows; row++)
        {
            const std::size_t i = indexAt(row, col);
            if (i >= rolls.size()) return;            // Rest of the grid is empty

            const RollEntry& r = rolls[i];
            attr_t attrs = 0;
            if (r.sevenOut)
                attrs = A_BOLD | COLOR_PAIR(colors.pair(Color::SevenOut));
            else if (r.passLineWinner)
                attrs = A_BOLD | COLOR_PAIR(colors.pair(Color::PassLineWinner));
            else if (r.point)
                attrs = A_BOLD | COLOR_PAIR(colors.pair(Color::Point));

            wattron(pWin_, attrs);
            mvwaddstr(pWin_, 3 + row, x, cellText(r).c_str());
            wattroff(pWin_, attrs);
        }
    }
}

//----------------------------------------------------------------

bool
DialogDeepRollHistory::handleKey(int ch)
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
DialogDeepRollHistory::back()
{
    setOperationResult(OperationResult::Acknowledged);
    SurfaceManager::instance().popSurface();
}

//----------------------------------------------------------------
