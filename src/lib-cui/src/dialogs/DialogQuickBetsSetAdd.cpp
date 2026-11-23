//----------------------------------------------------------------
//
// File: DialogQuickBetsSetAdd.cpp
//
//----------------------------------------------------------------

#include <cui/dialogs/DialogQuickBetsSetAdd.h>
#include <cui/managers/OperationManager.h>
#include <cui/managers/SurfaceManager.h>
#include <cui/core/CuiUtils.h>
#include <algorithm>
#include <cassert>

using namespace Cui;

namespace {
constexpr const char* NewText     = "[1] Set as new QuickBets entry";
constexpr const char* AddHeader   = "Add to an existing entry:";
constexpr const char* NoneText    = "You have no QuickBets entries yet.";
constexpr const char* BackText    = "[. or esc] Back";
constexpr const char* ScrollText  = "Up/Down shows more";
const std::string Separator(" | ");
} // namespace

//----------------------------------------------------------------

DialogQuickBetsSetAdd::DialogQuickBetsSetAdd()
    : DialogBase("DialogQuickBetsSetAdd")
{
    newWindow(winSize_.rows, winSize_.cols, winPos_.row, winPos_.col);
}

//----------------------------------------------------------------

DialogQuickBetsSetAdd&
DialogQuickBetsSetAdd::instance()
{
    static DialogQuickBetsSetAdd dialog;
    return dialog;
}

//----------------------------------------------------------------

void
DialogQuickBetsSetAdd::configure(std::string_view betText,
                                 std::vector<Entry> entries)
{
    title_ = "QuickBets Set or Add   (" + std::string(betText) + ")";
    if (entries.size() > maxEntries_)
    {
        entries.resize(maxEntries_);
    }
    entries_    = std::move(entries);
    top_        = 0;
    configured_ = true;
}

//----------------------------------------------------------------

void
DialogQuickBetsSetAdd::onAttach(SurfaceBase* pParent)
{
    assert(configured_);

    SurfaceBase::onAttach(pParent);
    setOperationResult(OperationResult::Unset);
    calcSize();
}

//----------------------------------------------------------------

void
DialogQuickBetsSetAdd::onDetach()
{
    SurfaceBase::onDetach();
    configured_ = false;          // Detect fresh configuration
}

//----------------------------------------------------------------
//
// Rows: border, title, rule, [1], rule, header, entries (or none text),
// back, border. Wide enough for the longest line, within the limit.
//
void
DialogQuickBetsSetAdd::calcSize()
{
    std::size_t nameWidth = 0;
    for (const auto& e : entries_)
    {
        nameWidth = std::max(nameWidth, e.name.size());
    }

    // "[a] " + name + " | " + description
    std::size_t listCols = 0;
    if (!entries_.empty())
    {
        listCols = 4 + nameWidth;
        std::size_t longestDesc = 0;
        for (const auto& e : entries_)
        {
            longestDesc = std::max(longestDesc, e.description.size());
        }
        if (longestDesc > 0 && listCols + Separator.size() < maxInnerCols_)
        {
            listCols += Separator.size() +
                        std::min(longestDesc,
                                 maxInnerCols_ - listCols - Separator.size());
        }
    }

    const std::size_t inner = std::max({title_.size(), listCols,
        std::string(NewText).size(), std::string(AddHeader).size(),
        std::string(NoneText).size(),
        std::string(BackText).size() + 1 + std::string(ScrollText).size()});

    const std::size_t shown = std::min(entries_.size(), maxVisible_);
    const std::size_t listRows = entries_.empty() ? 1 : shown;
    winSize_.rows = static_cast<int>(6 + listRows + 2);
    winSize_.cols = static_cast<int>(inner) + 4;
}

//----------------------------------------------------------------

LocationRequest
DialogQuickBetsSetAdd::getLocationRequest() const
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
DialogQuickBetsSetAdd::setLocation(WindowPosition pos)
{
    winPos_ = pos;
    resize(winSize_);
    repos(winPos_);
}

//----------------------------------------------------------------

void
DialogQuickBetsSetAdd::drawAll()
{
    werase(pWin_);
    box(pWin_, 0, 0);

    auto rule = [&](int row)
    {
        mvwhline(pWin_, row, 1, ACS_HLINE, winSize_.cols - 2);
        mvwaddch(pWin_, row, 0, ACS_LTEE);
        mvwaddch(pWin_, row, winSize_.cols - 1, ACS_RTEE);
    };

    mvwaddstr(pWin_, 1, 2, title_.c_str());
    rule(2);
    mvwaddstr(pWin_, 3, 2, NewText);
    rule(4);

    if (entries_.empty())
    {
        mvwaddstr(pWin_, 5, 2, NoneText);
    }
    else
    {
        mvwaddstr(pWin_, 5, 2, AddHeader);
    }

    const int innerCols = winSize_.cols - 4;
    std::size_t nameWidth = 0;
    for (const auto& e : entries_)
    {
        nameWidth = std::max(nameWidth, e.name.size());
    }

    const std::size_t shown = std::min(entries_.size(), maxVisible_);
    for (std::size_t line = 0; line < shown; ++line)
    {
        const std::size_t idx = top_ + line;
        if (idx >= entries_.size()) break;
        const Entry& e = entries_[idx];

        std::string text = std::string("[") +
                           static_cast<char>('a' + idx) + "] ";
        std::string name = e.name;
        name.resize(nameWidth, ' ');
        text += name;

        const std::size_t used = text.size() + Separator.size();
        if (!e.description.empty() && used < static_cast<std::size_t>(innerCols))
        {
            const std::size_t room = innerCols - used;
            std::string desc = e.description;
            if (desc.size() > room)
            {
                desc = (room > 3) ? desc.substr(0, room - 3) + "..."
                                  : desc.substr(0, room);
            }
            text += Separator + desc;
        }
        mvwaddstr(pWin_, 6 + static_cast<int>(line), 2, text.c_str());
    }

    const int backRow = winSize_.rows - 2;
    mvwaddstr(pWin_, backRow, 2, BackText);
    if (entries_.size() > maxVisible_)
    {
        const int col = winSize_.cols - 2 -
                        static_cast<int>(std::string(ScrollText).size());
        mvwaddstr(pWin_, backRow, col, ScrollText);
    }
}

//----------------------------------------------------------------

void
DialogQuickBetsSetAdd::draw()
{
    drawAll();
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------

void
DialogQuickBetsSetAdd::scrollList(int delta)
{
    if (entries_.size() <= maxVisible_) return;

    const int last = static_cast<int>(entries_.size() - maxVisible_);
    const int next = std::clamp(static_cast<int>(top_) + delta, 0, last);
    top_ = static_cast<std::size_t>(next);

    draw();
    doupdate();         // Arrow keys bypass the normal redraw
}

//----------------------------------------------------------------

bool
DialogQuickBetsSetAdd::handleKey(int ch)
{
    switch (ch)
    {
    case KEY_UP:   scrollList(-1); return true;
    case KEY_DOWN: scrollList(+1); return true;
    case '.':
    case 27:       cancel();   return true;
    case '1':      choose(NewEntry); return true;
    default:
        if (ch >= 'a' && ch <= 'z')
        {
            const std::size_t idx = static_cast<std::size_t>(ch - 'a');
            if (idx < entries_.size()) choose(idx);
            else                       beep();
            return true;
        }
        return false;
    }
}

//----------------------------------------------------------------

void
DialogQuickBetsSetAdd::choose(std::size_t index)
{
    setOperationResult(OperationResult::Success);
    SurfaceManager::instance().popSurface();
    OperationManager::instance().onQuickBetsChoice(true, index);
}

//----------------------------------------------------------------

void
DialogQuickBetsSetAdd::cancel()
{
    setOperationResult(OperationResult::Cancel);
    SurfaceManager::instance().popSurface();
    OperationManager::instance().onQuickBetsChoice(false, NewEntry);
}

//----------------------------------------------------------------
