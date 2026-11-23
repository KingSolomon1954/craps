//----------------------------------------------------------------
//
// File: DialogPicker.cpp
//
//----------------------------------------------------------------

#include <cui/dialogs/DialogPicker.h>
#include <cui/managers/OperationManager.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/core/CuiUtils.h>
#include <algorithm>
#include <cassert>
#include <cctype>

using namespace Cui;

namespace {
constexpr const char* ShowHint = "[. or enter or esc] Back";
constexpr std::size_t KeyCols = 4;       // "[a] " in front of each line
constexpr std::size_t MaxKeys = 26;      // a-z; later lines have no key
const std::string Separator(" | ");
} // namespace

//----------------------------------------------------------------

DialogPicker::DialogPicker()
    : DialogBase("DialogPicker")
{
    // Create an initial WINDOW at location 0,0. Gets resized later.
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
}

//----------------------------------------------------------------

DialogPicker&
DialogPicker::instance()
{
    static DialogPicker dialog;
    return dialog;
}

//----------------------------------------------------------------
//
// "Up/Down, Enter or a-f. Esc cancels", the letters being the ones
// that are in use.
//
std::string
DialogPicker::hint() const
{
    if (mode_ == Mode::Show)
    {
        return ShowHint;
    }

    const std::size_t keys = std::min(items_.size(), MaxKeys);
    std::string letters(1, 'a');
    if (keys > 1)
    {
        letters += "-";
        letters += static_cast<char>('a' + keys - 1);
    }
    return "Up/Down, Enter or " + letters + ". Esc cancels";
}

//----------------------------------------------------------------

void
DialogPicker::configure(std::string_view title,
                            std::vector<Item> items,
                            Mode mode)
{
    title_      = title;
    items_      = std::move(items);
    mode_       = mode;
    selected_   = 0;
    top_        = 0;
    configured_ = true;
}

//----------------------------------------------------------------

void
DialogPicker::onAttach(SurfaceBase* pParent)
{
    assert(configured_ && !items_.empty());

    SurfaceBase::onAttach(pParent);
    setOperationResult(OperationResult::Unset);
    calcSize();
}

//----------------------------------------------------------------

void
DialogPicker::onDetach()
{
    SurfaceBase::onDetach();
    configured_ = false;          // Detect fresh configuration
}

//----------------------------------------------------------------

void
DialogPicker::calcSize()
{
    textWidth_ = 0;
    noteWidth_ = 0;
    std::size_t longestDesc = 0;
    for (const auto& i : items_)
    {
        textWidth_  = std::max(textWidth_, i.text.size());
        noteWidth_  = std::max(noteWidth_, i.note.size());
        longestDesc = std::max(longestDesc, i.description.size());
    }

    const std::size_t numberCols = (mode_ == Mode::Pick) ? KeyCols : 0;
    const std::size_t hintCols   = hint().size();
    const std::size_t leftCols   = numberCols + textWidth_ +
                                   (noteWidth_ ? 1 + noteWidth_ : 0);

    // Room for descriptions after the bar, within the dialog limit.
    descWidth_ = 0;
    if (longestDesc > 0 && leftCols + Separator.size() < maxInnerCols_)
    {
        descWidth_ = std::min(longestDesc,
                              maxInnerCols_ - leftCols - Separator.size());
    }
    const std::size_t listCols = leftCols +
        (descWidth_ ? Separator.size() + descWidth_ : 0);
    const std::size_t inner    = std::max({title_.size(), listCols, hintCols});

    const std::size_t visible = std::min(items_.size(), maxVisible_);
    winSize_.rows = static_cast<int>(3 + visible + 3);  // title; list; hint
    winSize_.cols = static_cast<int>(inner) + 4;
}

//----------------------------------------------------------------

LocationRequest
DialogPicker::getLocationRequest() const
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
DialogPicker::setLocation(WindowPosition pos)
{
    winPos_ = pos;
    resize(winSize_);
    repos(winPos_);
}

//----------------------------------------------------------------
//
// 0   ┌────────────────────────────────────────┐
// 1   │ Switch to Player                       │
// 2   ├────────────────────────────────────────┤
// 3   │ [a] Nathan       | Primary player      │   selected is reversed
// 4   │ [b] Howie  (you) | Left-handed thrower │
// 5   ├────────────────────────────────────────┤
// 6   │ Up/Down, Enter or a-b. Esc cancels     │
// 7   └────────────────────────────────────────┘
//
void
DialogPicker::drawFrame()
{
    box(pWin_, 0, 0);
    mvwhline(pWin_, 2, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, winSize_.cols - 1, ACS_RTEE);

    mvwaddstr(pWin_, 1, 2, title_.c_str());

    const int hintSepRow = winSize_.rows - 3;
    mvwhline(pWin_, hintSepRow, 1, ACS_HLINE, winSize_.cols - 2);
    mvwaddch(pWin_, hintSepRow, 0, ACS_LTEE);
    mvwaddch(pWin_, hintSepRow, winSize_.cols - 1, ACS_RTEE);

    mvwaddstr(pWin_, winSize_.rows - 2, 2, hint().c_str());
}

//----------------------------------------------------------------

void
DialogPicker::drawItems()
{
    const std::size_t visible = std::min(items_.size(), maxVisible_);
    const int innerCols = winSize_.cols - 4;

    for (std::size_t line = 0; line < visible; ++line)
    {
        const std::size_t idx = top_ + line;
        if (idx >= items_.size()) break;
        const Item& item = items_[idx];

        std::string text;
        if (mode_ == Mode::Pick)
        {
            text = (idx < MaxKeys)
                 ? "[" + std::string(1, static_cast<char>('a' + idx)) + "] "
                 : "    ";
        }
        std::string name = item.text;
        name.resize(textWidth_, ' ');
        text += name;
        if (noteWidth_ > 0)
        {
            std::string note = item.note;
            note.resize(noteWidth_, ' ');
            text += " " + note;
        }
        if (descWidth_ > 0)
        {
            std::string desc = item.description;
            if (desc.size() > descWidth_)
            {
                desc = (descWidth_ > 3)
                    ? desc.substr(0, descWidth_ - 3) + "..."
                    : desc.substr(0, descWidth_);
            }
            text += Separator + desc;
        }
        text.resize(innerCols, ' ');

        const bool hot = (mode_ == Mode::Pick && idx == selected_);
        if (hot) wattron(pWin_, A_REVERSE);
        mvwaddstr(pWin_, 3 + static_cast<int>(line), 2, text.c_str());
        if (hot) wattroff(pWin_, A_REVERSE);
    }
}

//----------------------------------------------------------------

void
DialogPicker::draw()
{
    werase(pWin_);
    drawFrame();
    drawItems();
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------

void
DialogPicker::moveSelection(int delta)
{
    const int last = static_cast<int>(items_.size()) - 1;
    int next = static_cast<int>(selected_) + delta;
    next = std::clamp(next, 0, last);
    selected_ = static_cast<std::size_t>(next);

    // Keep the selection in view
    if (selected_ < top_) top_ = selected_;
    if (selected_ >= top_ + maxVisible_) top_ = selected_ + 1 - maxVisible_;

    draw();
    doupdate();         // Arrow keys bypass the normal redraw
}

//----------------------------------------------------------------

bool
DialogPicker::handleKey(int ch)
{
    switch (ch)
    {
    case KEY_UP:
        if (mode_ == Mode::Pick || items_.size() > maxVisible_) moveSelection(-1);
        return true;
    case KEY_DOWN:
        if (mode_ == Mode::Pick || items_.size() > maxVisible_) moveSelection(+1);
        return true;
    case '\r':
    case '\n':
    case KEY_ENTER:
        if (mode_ == Mode::Pick) choose(selected_);
        else                     closeWithoutChoice();
        return true;
    case '.':
    case 27:
        closeWithoutChoice();
        return true;
    default:
        if (mode_ == Mode::Pick && std::isalpha(ch) && ch < 128)
        {
            const std::size_t idx =
                static_cast<std::size_t>(std::tolower(ch) - 'a');
            if (idx < items_.size()) choose(idx);
            else                     beep();
            return true;
        }
        return false;
    }
}

//----------------------------------------------------------------

void
DialogPicker::choose(std::size_t index)
{
    const Craps::PlayerId id    = items_[index].playerId;
    const Craps::BetId    betId = items_[index].betId;
    const std::size_t     tag   = items_[index].tag;
    setOperationResult(OperationResult::Success);
    SurfaceManager::instance().popSurface();
    OperationManager::instance().onPickResult(true, id, betId, tag);
}

//----------------------------------------------------------------

void
DialogPicker::closeWithoutChoice()
{
    setOperationResult(OperationResult::Cancel);
    SurfaceManager::instance().popSurface();
    OperationManager::instance().onPickResult(false, Craps::PlayerId());
}

//----------------------------------------------------------------
