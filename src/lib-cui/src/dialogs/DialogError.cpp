//----------------------------------------------------------------
//
// File: DialogError.cpp
//
//----------------------------------------------------------------

#include <cui/dialogs/DialogError.h>
#include <cui/layouts/LayoutConsole.h>
#include <cui/managers/ColorManager.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/managers/OperationManager.h>
#include <cui/core/CuiUtils.h>
#include <gen/Logger.h>
#include <gen/StringUtils.h>
#include <wchar.h>

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
    LOG_TRACE("Exiting DialogError::configure()");
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
    et_.trace.clear();
}

//----------------------------------------------------------------

void
DialogError::buildDialogEntries(const Gen::ErrorPass& ep)
{
    formatTitle         (ep);
    formatDescription   (ep.description());
    formatProbableCause (ep.probableCause());
    formatProposedRepair(ep.proposedRepair());
    formatAdditionalInfo(ep.additionalInfo());
    formatTrace         (ep);
}

//----------------------------------------------------------------

void
DialogError::formatTitle(const Gen::ErrorPass& ep)
{
    Section& s = et_.title;  // convenience
    s.fmtdText.push_back(" " + ep.errorTypeStr() + ". " + ep.errorNameStr() + ".");
    int introLen = titleStr_.size() + 4;  // +4 for 2 emojis
    s.nCols = s.fmtdText[0].size() + introLen;
    s.nRows = s.fmtdText.size();
}

//----------------------------------------------------------------

void
DialogError::formatDescription(const std::string& description)
{
    Section& s = et_.desc;  // convenience

    wrapString(description, s.fmtdText); 
    int introLen = descriptionStr_.size() + 3;  // +3 for emoji + space
    s.nCols = std::max(maxLength(s.fmtdText), introLen);
    s.nRows = s.fmtdText.size() + 1; // +1 for intro;
}

//----------------------------------------------------------------

void
DialogError::formatProbableCause(const std::string& probableCause)
{
    if (probableCause.empty()) return;  // Skip empty section
    
    Section& s = et_.cause;  // convenience

    wrapString(probableCause, s.fmtdText);
    int introLen = probableCauseStr_.size() + 3;  // +3 for emoji + space
    s.nCols = std::max(maxLength(s.fmtdText), introLen);
    s.nRows = s.fmtdText.size() + 1; // +1 for intro
}

//----------------------------------------------------------------

void
DialogError::formatProposedRepair(const std::string& proposedRepair)
{
    if (proposedRepair.empty()) return;  // Skip empty section
    
    Section& s = et_.repair;  // convenience

    wrapString(proposedRepair, s.fmtdText); 
    int introLen = proposedRepairStr_.size() + 3;  // +3 for emoji + space
    s.nCols = std::max(maxLength(s.fmtdText), introLen);
    s.nRows = s.fmtdText.size() + 1; // +1 for intro
}

//----------------------------------------------------------------

void
DialogError::formatAdditionalInfo(const std::string& additionalInfo)
{
    if (additionalInfo.empty()) return;  // Skip empty section
    
    Section& s = et_.addinfo;  // convenience

    wrapString(additionalInfo, s.fmtdText); 
    int introLen = additionalInfoStr_.size() + 3;  // +3 for emoji + space
    s.nCols = std::max(maxLength(s.fmtdText), introLen);
    s.nRows = s.fmtdText.size() + 1; // +1 for intro
}

//----------------------------------------------------------------

void
DialogError::formatTrace(const Gen::ErrorPass& ep)
{
    using E = Gen::ErrorPass::ErrorType;
    
    if (ep.errorType() != ep.errorTypeStr(E::ProgrammingError) ||
        ep.stackTrace().size() == 0) return;  // Skip empty section
    
    Section& s = et_.trace;  // convenience

    wrapStrings(ep.stackTrace(), s.fmtdText); 
    int introLen = traceStr_.size() + 3;  // +3 for emoji + space
    s.nCols = std::max(maxLength(s.fmtdText), introLen);
    s.nRows = s.fmtdText.size() + 1; // +1 for intro
}

//----------------------------------------------------------------

int
DialogError::wrapWidth()
{
    constexpr int preferredWidth = 76;
    constexpr int screenMargin = 4;

    const int maxWidth = std::max(1, LayoutConsole::minCols - screenMargin);
    return std::min(preferredWidth, maxWidth);
}

//----------------------------------------------------------------

void
DialogError::wrapString(const std::string& s, Aos& result)
{
    Gen::StringUtils::wrapFormattedString(s, wrapWidth(), result);
}

//----------------------------------------------------------------

void
DialogError::wrapStrings(const Aos& source, Aos& result)
{
    Gen::StringUtils::wrapFormattedStrings(source, wrapWidth(), result);
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

    int nCols = maxNumCols();     // Max col of all sections, including intro labels
    winSize_.cols = nCols + borderCols + blankCols;

    // Rows
    constexpr int borderRows  = 2; // Top + bottom border
    constexpr int promptRows  = 2; // Prompt row + horizontal line above it

    auto calcRows = [&]()
    {
        const int linesBetweenSects = std::max(0, numSections() - 1);
        return numRows() + borderRows + promptRows + linesBetweenSects;
    };

    winSize_.rows = calcRows();

    // If necessary, trim the trace from the bottom until the dialog fits.
    while (winSize_.rows > LayoutConsole::minRows &&
           !et_.trace.fmtdText.empty())
    {
        et_.trace.fmtdText.pop_back();

        // Keep the trace metadata consistent with its formatted text.
        et_.trace.nRows = static_cast<int>(et_.trace.fmtdText.size());
        et_.trace.nCols = maxLength(et_.trace.fmtdText);

        // Recalculate because the trace section may now be empty.
        winSize_.rows = calcRows();
    }

    LOG_TRACE("DialogError::calcSize():"
              " winSize_.rows = " + std::to_string(winSize_.rows) +
              " winSize_.cols = " + std::to_string(winSize_.cols));
}

//----------------------------------------------------------------

int
DialogError::numRows()
{
    return et_.title.nRows   +
           et_.desc.nRows    +
           et_.cause.nRows   +
           et_.repair.nRows  +
           et_.addinfo.nRows +
           et_.trace.nRows;
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

int
DialogError::maxLength(const Aos& vos)
{
    size_t maxLen = 0;
    for (const auto& l : vos)   // For each string in vector of strings
    {
        maxLen = std::max(maxLen, l.size());
    }
    return maxLen;
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
DialogError::drawTitle(int startRow)
{
    int nRows = 0;
    nRows += drawIntroTitle(startRow);
    waddstr(pWin_, et_.title.fmtdText[0].c_str());  // Add to end of intro
    nRows += drawSectionSep(startRow + nRows);
    return nRows;
}

//----------------------------------------------------------------

int
DialogError::drawDesc(int startRow)
{
    if (et_.desc.nCols == 0) return 0;  // Skip empty section
    
    int nRows = 0;
    nRows += drawIntroDesc(startRow);
    nRows += drawFmtdText(startRow, et_.desc.fmtdText);
    nRows += drawSectionSep(startRow + nRows);
    return nRows;
}

//----------------------------------------------------------------

int
DialogError::drawCause(int startRow)
{
    if (et_.cause.nCols == 0) return 0;  // Skip empty section
    
    int nRows = 0;
    nRows += drawIntroCause(startRow);
    nRows += drawFmtdText(startRow, et_.cause.fmtdText);
    nRows += drawSectionSep(startRow + nRows);
    return nRows;
}

//----------------------------------------------------------------

int
DialogError::drawRepair(int startRow)
{
    if (et_.repair.nCols == 0) return 0;  // Skip empty section
    
    int nRows = 0;
    nRows += drawIntroRepair(startRow);
    nRows += drawFmtdText(startRow, et_.repair.fmtdText);
    nRows += drawSectionSep(startRow + nRows);
    return nRows;
}

//----------------------------------------------------------------

int
DialogError::drawAddinfo(int startRow)
{
    if (et_.addinfo.nCols == 0) return 0;  // Skip empty section
    
    int nRows = 0;
    nRows += drawIntroAddinfo(startRow);
    nRows += drawFmtdText(startRow, et_.addinfo.fmtdText);
    nRows += drawSectionSep(startRow + nRows);
    return nRows;
}

//----------------------------------------------------------------

int
DialogError::drawTrace(int startRow)
{
    if (et_.trace.nCols == 0) return 0;  // Skip empty section
    
    int nRows = 0;
    nRows += drawIntroTrace(startRow);
    nRows += drawFmtdText(startRow, et_.trace.fmtdText);
    nRows += drawSectionSep(startRow + nRows);
    return nRows;
}

//----------------------------------------------------------------

int
DialogError::drawPrompt(int startRow)
{
    mvwaddstr(pWin_, startRow, 2, "[. or esc or enter] OK");
    
    return 1;  // How many rows were drawn
}

//----------------------------------------------------------------

int
DialogError::drawIntroTitle(int startRow)
{
    const std::wstring left   = L"\U0000274C";  // ❌ U+274C
    const std::wstring right  = L"\U00002755";  // ❕ U+2755
    const std::wstring middle = titleStr_;
    const std::wstring label  = left + middle + right;

    attrTitle(true);
    mvwaddwstr(pWin_, startRow, 2, label.data());
    attrTitle(false);
    return 1;
}

//----------------------------------------------------------------

int
DialogError::drawIntroDesc(int startRow)
{
    const std::wstring left   = L"\U0001F4DC";      // 📜 U+1F4DC
    const std::wstring right  = L" Description:";
    const std::wstring label  = left + right;

    attrDesc(true);
    mvwaddwstr(pWin_, startRow, 2, label.data());
    attrDesc(false);
    return 1;
}
    
//----------------------------------------------------------------

int
DialogError::drawIntroCause(int startRow)
{
    const std::wstring left   = L"\U0001F4C1";      // 📂 U+1F4C1
    const std::wstring right  = probableCauseStr_;
    const std::wstring label  = left + right;

    attrCause(true);
    mvwaddwstr(pWin_, startRow, 2, label.data());
    attrCause(false);
    return 1;
}
    
//----------------------------------------------------------------

int
DialogError::drawIntroRepair(int startRow)
{
    const std::wstring left   = L"\U0001F528";      // 🔨 U+1F528
    const std::wstring right  = proposedRepairStr_;
    const std::wstring label  = left + right;

    attrRepair(true);
    mvwaddwstr(pWin_, startRow, 2, label.data());
    attrRepair(false);
    return 1;
}
    
//----------------------------------------------------------------

int
DialogError::drawIntroAddinfo(int startRow)
{
    const std::wstring left   = L"\U0001F506";      // 🔆 U+1F506
    const std::wstring right  = additionalInfoStr_;
    const std::wstring label  = left + right;

    attrAddinfo(true);
    mvwaddwstr(pWin_, startRow, 2, label.data());
    attrAddinfo(false);
    return 1;
}
    
//----------------------------------------------------------------

int
DialogError::drawIntroTrace(int startRow)
{
    const std::wstring left   = L"\U0001F447";      // 👇 U+1F447
    const std::wstring right  = traceStr_;
    const std::wstring label  = left + right;

    attrTrace(true);
    mvwaddwstr(pWin_, startRow, 2, label.data());
    attrTrace(false);
    return 1;
}
    
//----------------------------------------------------------------

void
DialogError::attrTitle(bool onOff)
{
    if (onOff == true)
    {
        wattron(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogErrorTitle)));
    }
    else
    {
        wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogErrorTitle)));
    }
}

//----------------------------------------------------------------

void
DialogError::attrDesc(bool onOff)
{
    if (onOff == true)
    {
        wattron(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogErrorDesc)));
    }
    else
    {
        wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogErrorDesc)));
    }
}

//----------------------------------------------------------------

void
DialogError::attrCause(bool onOff)
{
    if (onOff == true)
    {
        wattron(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogErrorCause)));
    }
    else
    {
        wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogErrorCause)));
    }
}

//----------------------------------------------------------------

void
DialogError::attrRepair(bool onOff)
{
    if (onOff == true)
    {
        wattron(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogErrorRepair)));
    }
    else
    {
        wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogErrorRepair)));
    }
}

//----------------------------------------------------------------

void
DialogError::attrAddinfo(bool onOff)
{
    if (onOff == true)
    {
        wattron(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogErrorAddinfo)));
    }
    else
    {
        wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogErrorAddinfo)));
    }
}

//----------------------------------------------------------------

void
DialogError::attrTrace(bool onOff)
{
    if (onOff == true)
    {
        wattron(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogErrorTrace)));
    }
    else
    {
        wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::DialogErrorTrace)));
    }
}

//----------------------------------------------------------------

int
DialogError::drawSectionSep(int startRow)
{
    mvwhline(pWin_, startRow, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, startRow, 0, ACS_LTEE);
    mvwaddch(pWin_, startRow, winSize_.cols - 1, ACS_RTEE);
    return 1;  // How many rows were drawn
}

//----------------------------------------------------------------

int
DialogError::drawFmtdText(int startRow, const Aos& text)
{
    int row = startRow;
    for (const auto& line : text)
    {
        mvwprintw(pWin_, row++, 2, "%s", line.c_str());
    }
    
    return text.size();
}

//----------------------------------------------------------------

bool
DialogError::handleKey(int ch)
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
DialogError::back()
{
    setOperationResult(OperationResult::Acknowledged);
    SurfaceManager::instance().popSurface();
    OperationManager::instance().onErrorAcknowledged();
}

//----------------------------------------------------------------
