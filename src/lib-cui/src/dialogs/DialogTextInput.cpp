//----------------------------------------------------------------
//
// File: DialogTextInput.cpp
//
//----------------------------------------------------------------

#include <cui/dialogs/DialogTextInput.h>
#include <cui/managers/ColorManager.h>
#include <cui/managers/OperationManager.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/core/CuiUtils.h>
#include <algorithm>
#include <cassert>

using namespace Cui;

namespace {
constexpr const char* Hint = "[enter] OK   [esc] Cancel";
} // namespace

//----------------------------------------------------------------

DialogTextInput::DialogTextInput()
    : DialogBase("DialogTextInput")
{
    // Create an initial WINDOW at location 0,0. Gets resized later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
}

//----------------------------------------------------------------

DialogTextInput&
DialogTextInput::instance()
{
    static DialogTextInput dialog;
    return dialog;
}

//----------------------------------------------------------------

void
DialogTextInput::configure(std::string_view title,
                           std::string_view prompt,
                           std::string_view initial,
                           std::size_t      maxChars,
                           Validator        validator)
{
    title_     = title;
    prompt_    = prompt;
    maxChars_  = maxChars;
    validator_ = std::move(validator);
    message_.clear();

    lineInput_ = LineInput(maxChars_, LineInput::Charset::PlayerName);
    lineInput_.setValue(std::string(initial.substr(0, maxChars_)));
    configured_ = true;
}

//----------------------------------------------------------------

void
DialogTextInput::onAttach(SurfaceBase* pParent)
{
    assert(configured_);          // Must first call configure()

    SurfaceBase::onAttach(pParent);
    setOperationResult(OperationResult::Unset);
    calcSize();
    curs_set(1);
}

//----------------------------------------------------------------

void
DialogTextInput::onDetach()
{
    SurfaceBase::onDetach();
    curs_set(0);
    configured_ = false;          // Detect fresh configuration
}

//----------------------------------------------------------------

void
DialogTextInput::calcSize()
{
    constexpr int borderCols = 2;
    constexpr int blankCols  = 2;

    inputRow_ = 3;
    inputCol_ = 2 + static_cast<int>(prompt_.size());

    const std::size_t inner = std::max<std::size_t>(
        { title_.size(),
          prompt_.size() + maxChars_,
          std::string(Hint).size() });

    winSize_.rows = rows_;
    winSize_.cols = static_cast<int>(inner) + blankCols + borderCols;
}

//----------------------------------------------------------------

LocationRequest
DialogTextInput::getLocationRequest() const
{
    LocationRequest req;
    req.kind      = LocationKind::Dialog;
    req.size.rows = winSize_.rows;
    req.size.cols = winSize_.cols;
    req.direction = Direction::Right;
    return req;
}

//----------------------------------------------------------------

void
DialogTextInput::setLocation(WindowPosition pos)
{
    winPos_ = pos;
    resize(winSize_);
    repos(winPos_);
}

//----------------------------------------------------------------
//
// 0   ┌────────────────────────────┐
// 1   │ Rename Player              │
// 2   ├────────────────────────────┤
// 3   │ Name: Nathan______         │
// 4   │ A name can be at most 16.  │
// 5   │ [enter] OK   [esc] Cancel  │
// 6   └────────────────────────────┘
//
void
DialogTextInput::drawFrame()
{
    box(pWin_, 0, 0);
    mvwhline(pWin_, 2, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, winSize_.cols - 1, ACS_RTEE);

    mvwaddstr(pWin_, 1, 2, title_.c_str());
    mvwaddstr(pWin_, inputRow_, 2, prompt_.c_str());
    mvwaddstr(pWin_, 5, 2, Hint);

    if (!message_.empty())
    {
        const short pair =
            ColorManager::instance().pair(Color::DialogMessageWarn);
        wattron(pWin_, COLOR_PAIR(pair));
        mvwaddstr(pWin_, 4, 2, message_.substr(0, winSize_.cols - 4).c_str());
        wattroff(pWin_, COLOR_PAIR(pair));
    }
}

//----------------------------------------------------------------
//
// The field is maxChars_ wide. Unused positions show as underscores.
//
void
DialogTextInput::drawInput()
{
    std::string field = lineInput_.value();
    if (field.size() < maxChars_)
    {
        field.append(maxChars_ - field.size(), '_');
    }
    mvwaddstr(pWin_, inputRow_, inputCol_, field.c_str());
    wmove(pWin_, inputRow_,
          inputCol_ + static_cast<int>(lineInput_.cursorPosition()));
    wnoutrefresh(pWin_);
}

//----------------------------------------------------------------

void
DialogTextInput::draw()
{
    werase(pWin_);
    drawFrame();
    drawInput();
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------

bool
DialogTextInput::handleKey(int ch)
{
    switch (ch)
    {
    case '\r':
    case '\n':
    case KEY_ENTER: doEnter();  return true;
    case 27:        doCancel(); return true;
    default:
        if (lineInput_.handleKey(ch))
        {
            message_.clear();
            werase(pWin_);
            drawFrame();
            drawInput();
            CuiUtils::transfer(pWin_);
            doupdate();          // This key bypasses the normal redraw
            return true;
        }
        return false;
    }
}

//----------------------------------------------------------------

void
DialogTextInput::doEnter()
{
    if (validator_)
    {
        message_ = validator_(lineInput_.value());
        if (!message_.empty())
        {
            beep();
            werase(pWin_);
            drawFrame();
            drawInput();
            CuiUtils::transfer(pWin_);
            doupdate();
            return;
        }
    }

    const std::string text = lineInput_.value();
    setOperationResult(OperationResult::Success);
    SurfaceManager::instance().popSurface();
    OperationManager::instance().onTextResult(true, text);
}

//----------------------------------------------------------------

void
DialogTextInput::doCancel()
{
    setOperationResult(OperationResult::Cancel);
    SurfaceManager::instance().popSurface();
    OperationManager::instance().onTextResult(false, std::string());
}

//----------------------------------------------------------------
