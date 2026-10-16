//----------------------------------------------------------------
//
// File: DialogError.cpp
//
//----------------------------------------------------------------

#include <cui/dialogs/DialogError.h>
#include <cui/layouts/LayoutConsole.h>
#include <cui/managers/ColorManager.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/core/CuiUtils.h>
#include <gen/Logger.h>
#include <gen/StringUtils.h>  // For wrapText()
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

DialogError::DialogError()
    : DialogBase("DialogError")
{
    // Create an initial WINDOW at location 0,0. Gets resized later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
}

//----------------------------------------------------------------

DialogError&
DialogError::instance()
{
    static DialogError dae;
    return dae;
}

/*-----------------------------------------------------------*//**

Caller provides what is to be displayed.

This call populates ErrorText from ErrorPass.

Upon return, ErrorText is formatted and ready to be drawn.

ErrorText structure knows the number of sections to be drawn. Each
section is already formatted (wrapped as necessary), max rows and cols
of each section are known. winPos_ and winSize_ have been updated.

*/
void
DialogError::configure(const Gen::ErrorPass& ep)
{
    eraseFields();
    buildDialogEntries(ep);
    calcSize();
}

//----------------------------------------------------------------

void
DialogError::eraseFields()
{
    et_.title.clear();
    et_.desc.clear();
    et_.cause.clear();
    et_.repair.clear();
    et_.addinfo.clear();
}

//----------------------------------------------------------------

void
DialogError::buildDialogEntries(const Gen::ErrorPass& ep)
{
    formatDescription(ep.description());
    formatProbableCause(ep.probableCause());
    formatProposedRepair(ep.proposedRepair());
    formatAdditionalInfo(ep.additionalInfo());
    formatTrace(ep.stackTrace());    
}

//----------------------------------------------------------------

void
DialogError::formatTitle(const std::string& errorName,
                         const std::string& errorType)
{
    et_.title.fmtdText[0] = "Title Section";
    et_.title.nCols = et_.title.fmtdText[0].size();
    et_.title.nRows = et_.title.fmtdText.size();
}

//----------------------------------------------------------------

void
DialogError::formatDescription(const std::string& description)
{
    et_.desc.fmtdText[0] = "Description Section";
    et_.desc.nCols = et_.desc.fmtdText[0].size();
    et_.desc.nRows = et_.desc.fmtdText.size();
}

//----------------------------------------------------------------

void
DialogError::formatProbableCause(const std::string& probableCause)
{
    et_.cause.fmtdText[0] = "Probable Cause Section";
    et_.cause.nCols = et_.cause.fmtdText[0].size();
    et_.cause.nRows = et_.cause.fmtdText.size();
}

//----------------------------------------------------------------

void
DialogError::formatProposedRepair(const std::string& proposedRepair)
{
    et_.repair.fmtdText[0] = "Proposed Repair Section";
    et_.repair.nCols = et_.repair.fmtdText[0].size();
    et_.repair.nRows = et_.repair.fmtdText.size();
}

//----------------------------------------------------------------

void
DialogError::formatAdditionalInfo(const std::string& additionalInfo)
{
    et_.addinfo.fmtdText[0] = "Additional Info Section";
    et_.addinfo.nCols = et_.addinfo.fmtdText[0].size();
    et_.addinfo.nRows = et_.addinfo.fmtdText.size();
}

//----------------------------------------------------------------

void
DialogError::formatTrace(const Aos& stackTrace)
{
    et_.trace.fmtdText[0] = "Trace Section";
    et_.trace.nCols = et_.trace.fmtdText[0].size();
    et_.trace.nRows = et_.trace.fmtdText.size();
}

//----------------------------------------------------------------
//
// Set winSize_
//
void
DialogError::calcSize()
{
    // Columns
    constexpr int borderCols = 2; // 2 vertical borders
    constexpr int blankCols  = 2; // 2 empty columns adjacent to borders

    int nCols = maxNumCols();     // Max col of all formatted sections
    winSize_.cols = nCols + borderCols + blankCols;

    // Rows
    constexpr int borderRows  = 2; // Top + bot border
    constexpr int promptRows = 2;  // Prompt row + horz line above it
    int nSections = numSections();
    int nRows     = numRows();     // Num rows of all formatted sections
    winSize_.rows = borderRows + promptRows + (nSections - 1);
    
    LOG_TRACE("DialogError::calcSize(): winSize_.rows = " +
              std::to_string(winSize_.rows) +  " winSize_.cols = " +
              std::to_string(winSize_.cols));
}

//----------------------------------------------------------------

int
DialogError::numRows()
{
    int nRows = 0;
    nRows = std::max(nRows, et_.title.nRows);
    nRows = std::max(nRows, et_.desc.nRows);
    nRows = std::max(nRows, et_.cause.nRows);
    nRows = std::max(nRows, et_.repair.nRows);
    nRows = std::max(nRows, et_.addinfo.nRows);
    nRows = std::max(nRows, et_.trace.nRows);
    return nRows;
}

//----------------------------------------------------------------

int
DialogError::maxNumCols()
{
    int nCols = 0;
    nCols = std::max(nCols, et_.title.nCols);
    nCols = std::max(nCols, et_.desc.nCols);
    nCols = std::max(nCols, et_.cause.nCols);
    nCols = std::max(nCols, et_.repair.nCols);
    nCols = std::max(nCols, et_.addinfo.nCols);
    nCols = std::max(nCols, et_.trace.nCols);
    return nCols;
}
    
//----------------------------------------------------------------

int
DialogError::numSections()
{
    int nSects = 0;
    if (et_.title.nCols   > 0) nSects++;
    if (et_.desc.nCols    > 0) nSects++;
    if (et_.cause.nCols   > 0) nSects++;
    if (et_.repair.nCols  > 0) nSects++;
    if (et_.addinfo.nCols > 0) nSects++;
    if (et_.trace.nCols   > 0) nSects++;
    return nSects;
}
    
//----------------------------------------------------------------
//
// SurfaceManager wants our window size and more.
// This occurs in context of SurfaceManager::pushSurface()
// SurfaceManager informs us shortly of our screen position.
// See setLocation() below. 
//
LocationRequest
DialogError::getLocationRequest() const
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
DialogError::setLocation(WindowPosition pos)
{
    winPos_ = pos;
    resize(winSize_);  // Dynamic menu needs resizing
    repos(winPos_);    // and repositioning
}

//----------------------------------------------------------------
//
//    0123456789 123456789 123456789 123456789 123456789 1234567890 
// 0  ┌───────────────────────────────────────────────────────────┐
// 1  │ ❌ ERROR❗ Processing Error. Item Already Exists.         │
// 2  ├───────────────────────────────────────────────────────────┤
// 3  │ 📜 Description:                                           │
// 4  │                                                           │
// 5  ├───────────────────────────────────────────────────────────┤
// 6  │ 📂 Probable Cause:                                        │
// 7  │                                                           │
// 8  ├───────────────────────────────────────────────────────────┤
// 9  │ 🔨 Proposed Repair:                                       │
// 10 │                                                           │
// 11 ├───────────────────────────────────────────────────────────┤
// 12 │ 🔆 Additional Info:                                       │
// 13 │                                                           │
// 14 ├───────────────────────────────────────────────────────────┤
// 15 │ 👇 Trace                                                  │
// 16 │                                                           │
// 17 ├───────────────────────────────────────────────────────────┤
// 18 │ [. or esc] OK                                             │
// 19 └───────────────────────────────────────────────────────────┘
//
void
DialogError::draw()
{
    werase(pWin_);

    box(pWin_, 0, 0);
    
    int row = 1;
    row += drawTitle  (row);
    row += drawDesc   (row);
    row += drawCause  (row);
    row += drawRepair (row);
    row += drawAddinfo(row);
    row += drawTrace  (row);
    row += drawPrompt (row);

    CuiUtils::transfer(pWin_);
}
    
//----------------------------------------------------------------

int
DialogError::drawTitle(int startingRow)
{
    // draw title
    mvwaddstr(pWin_, startingRow, 2, et_.title.fmtdText[0].c_str());

    // draw bottom horz border + side junctions
    mvwhline(pWin_, startingRow + 1, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, startingRow + 1, 0, ACS_LTEE);
    mvwaddch(pWin_, startingRow + 1, winSize_.cols - 1, ACS_RTEE);
    
    return 2;  // How many rows were drawn
}

//----------------------------------------------------------------

int
DialogError::drawDesc(int startingRow)
{
    mvwaddstr(pWin_, startingRow, 2, et_.desc.fmtdText[0].c_str());

    // draw bottom horz border + side junctions
    mvwhline(pWin_, startingRow + 1, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, startingRow + 1, 0, ACS_LTEE);
    mvwaddch(pWin_, startingRow + 1, winSize_.cols - 1, ACS_RTEE);
    
    return 2;  // How many rows were drawn
}

//----------------------------------------------------------------

int
DialogError::drawCause(int startingRow)
{
    mvwaddstr(pWin_, startingRow, 2, et_.cause.fmtdText[0].c_str());

    // draw bottom horz border + side junctions
    mvwhline(pWin_, startingRow + 1, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, startingRow + 1, 0, ACS_LTEE);
    mvwaddch(pWin_, startingRow + 1, winSize_.cols - 1, ACS_RTEE);
    
    return 2;  // How many rows were drawn
}

//----------------------------------------------------------------

int
DialogError::drawRepair(int startingRow)
{
    mvwaddstr(pWin_, startingRow, 2, et_.repair.fmtdText[0].c_str());

    // draw bottom horz border + side junctions
    mvwhline(pWin_, startingRow + 1, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, startingRow + 1, 0, ACS_LTEE);
    mvwaddch(pWin_, startingRow + 1, winSize_.cols - 1, ACS_RTEE);
    
    return 2;  // How many rows were drawn
}

//----------------------------------------------------------------

int
DialogError::drawAddinfo(int startingRow)
{
    mvwaddstr(pWin_, startingRow, 2, et_.addinfo.fmtdText[0].c_str());

    // draw bottom horz border + side junctions
    mvwhline(pWin_, startingRow + 1, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, startingRow + 1, 0, ACS_LTEE);
    mvwaddch(pWin_, startingRow + 1, winSize_.cols - 1, ACS_RTEE);
    
    return 2;  // How many rows were drawn
}

//----------------------------------------------------------------

int
DialogError::drawTrace(int startingRow)
{
    mvwaddstr(pWin_, startingRow, 2, et_.trace.fmtdText[0].c_str());

    // draw bottom horz border + side junctions
    mvwhline(pWin_, startingRow + 1, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, startingRow + 1, 0, ACS_LTEE);
    mvwaddch(pWin_, startingRow + 1, winSize_.cols - 1, ACS_RTEE);
    
    return 2;  // How many rows were drawn
}

//----------------------------------------------------------------

int
DialogError::drawPrompt(int startingRow)
{
    mvwaddstr(pWin_, startingRow, 2, "[. or esc or enter] OK");
    
    return 1;  // How many rows were drawn
}

//----------------------------------------------------------------

bool
DialogError::handleKey(int ch)
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
DialogError::back()
{
    setOperationResult(OperationResult::Acknowledged);
    SurfaceManager::instance().popSurface();
}

//----------------------------------------------------------------



//----------------------------------------------------------------

#if 0

// For Wrapping text

    const int maxDialogWidth = 80;

    // Wrap message into multiple lines (max width = 76 chars for padding)
    int wrapWidth = std::min(maxDialogWidth - 4, screenCols - 4);
    std::vector<std::string> lines = Gen::StringUtils::wrapText(message_, wrapWidth);
    int msgLines = static_cast<int>(lines.size());

    // Box dimensions
    int boxWidth = std::min(maxDialogWidth, screenCols - 2);
    int boxHeight = msgLines + 4; // border + message lines + prompt

#endif    

