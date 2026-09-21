//----------------------------------------------------------------
//
// File: WindowRollHistory.cpp
//
//----------------------------------------------------------------

#include <cui/panels/WindowRollHistory.h>
#include <cui/layouts/LayoutCrapsScreen.h>
#include <cui/CuiUtils.h>
#include <cui/CuiStructs.h>
#include <gen/Logger.h>

using namespace Cui;

//----------------------------------------------------------------

WindowRollHistory::WindowRollHistory()
    : PanelBase("WindowRollHistory")
{
    newWindow(Layout::rollHistHeight,           // In base class
              Layout::rollHistWidth,
              Layout::rollHistTopRow,
              Layout::rollHistLeftCol);
}

//----------------------------------------------------------------

WindowRollHistory&
WindowRollHistory::instance()
{
    static WindowRollHistory rh;
    return rh;
}

//----------------------------------------------------------------

void
WindowRollHistory::draw()
{
    werase(pWin_);

    drawExternalJunctions();
    drawInternalBorders();
    drawStaticContent();
    populate();
    
    CuiUtils::transfer(pWin_);
}

//----------------------------------------------------------------
//
// We need to touch up border junctions to mate with our internal
// lines. But the border is outside of our window. Ask LayoutCrapsTable
// to take of it.
//
void
WindowRollHistory::drawExternalJunctions()
{
    // Have none
}

//----------------------------------------------------------------

void
WindowRollHistory::drawInternalBorders()
{
    // Have none
}

//----------------------------------------------------------------
//
// Draw static field contents
//
void
WindowRollHistory::drawStaticContent()
{
    // Have none
}

//----------------------------------------------------------------
//
// Updates all dynamic field content.
//
// String will usually be a couple of chars longer than CharLimit,
// but the WINDOW will cut off the extra. This is actually
// desired, so partial next rolls are visible.
//
void
WindowRollHistory::populate()
{
    // TODO: look up the user-configured format.
    constexpr Format fmtConfig = Format::D;

    int column = 1;
    const int rightColumn = 1 + static_cast<int>(CharLimit);

    for (const auto& r : rolls_)
    {
        const std::string text = format(fmtConfig, r);

        if (column >= rightColumn)
            break;

        const int remaining = rightColumn - column;
        const int count = std::min(
            remaining,
            static_cast<int>(text.size())
        );

        if (r.sevenOut)
        {
            wattron(pWin_, A_BOLD | COLOR_PAIR(ColorPairs::SevenOut));
        }
        else if (r.passLineWinner)
        {
            wattron(pWin_, A_BOLD | COLOR_PAIR(ColorPairs::PassLineWinner));
        }

        mvwaddnstr(pWin_, 0, column, text.c_str(), count);

        if (r.sevenOut)
        {
            wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorPairs::SevenOut));
        }
        else if (r.passLineWinner)
        {
            wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorPairs::PassLineWinner));
        }

        column += count;
    }
}

#if 0
void
WindowRollHistory::populate()
{
    // TODO lookup user configured format, assume something for now
    Format fmtConfig = Format::D;
    
    std::string s;
    for (const auto& r : rolls_)
    {
        s += format(fmtConfig, r);
        if (s.size() > CharLimit)
        {
            break;
        }
    }
    mvwprintw(pWin_, 0, 1, s.c_str());
}
#endif

//----------------------------------------------------------------
//
// │ [A]   7(3,4) 8(4,4) 6(3,3) 11(6,5)  │
// │ [B]   7(34) 8(44) 6(33) 11(65)      │
// │ [C]   7,8,6,11                      │
// │ [D]   7 8 6 11                      │
//
std::string
WindowRollHistory::format(Format fmt, const Roll& r) const
{
    std::string d1_s (std::to_string(r.d1));
    std::string d2_s (std::to_string(r.d2));
    std::string val_s(std::to_string(r.d1 + r.d2));
    
    std::string rollFmtd;
    if (fmt == Format::A)
    {
        rollFmtd = val_s + "(" + d1_s + "," + d2_s + ") ";
    }
    if (fmt == Format::B)
    {
        rollFmtd = val_s + "(" + d1_s + d2_s + ") ";
    }
    if (fmt == Format::C)
    {
        rollFmtd = val_s + ",";
    }
    if (fmt == Format::D)
    {
        rollFmtd = val_s + " ";
    }
    
    return rollFmtd;
}

//----------------------------------------------------------------

void
WindowRollHistory::onDiceNewValue(unsigned d1, unsigned d2, unsigned rollCount)
{
    (void) rollCount;
    
    add(Roll{d1, d2});
}

//----------------------------------------------------------------

void
WindowRollHistory::onSevenOut()
{
    // Retro-actively mark the front element indicating seven out.
    rolls_[0].sevenOut = true;
}

//----------------------------------------------------------------

void
WindowRollHistory::onPassLineWinner()
{
    // Retro-actively mark the front element indicating pass line winner.
    rolls_[0].passLineWinner = true;
}

//----------------------------------------------------------------

void
WindowRollHistory::add(const Roll& r)
{
    rolls_.push_front(r);

    if (rolls_.size() > MaxSize)
    {
        rolls_.pop_back();  // remove oldest
    }
}

//----------------------------------------------------------------

const WindowRollHistory::Roll&
WindowRollHistory::operator[](std::size_t index) const
{
    return rolls_[index];
}    

//----------------------------------------------------------------

#if 0
// In future when doing scrolling feature for Roll history window
    std::size_t visibleCount = 10;
    std::size_t firstVisible = 0;
    std::size_t lastVisible = std::min(firstVisible + visibleCount,
                                       history.size());

    for (std::size_t i = firstVisible; i < lastVisible; ++i)
    {
        displayRoll(history[i]);
    }
#endif

