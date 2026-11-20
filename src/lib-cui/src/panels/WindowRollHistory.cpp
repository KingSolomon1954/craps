//----------------------------------------------------------------
//
// File: WindowRollHistory.cpp
//
//----------------------------------------------------------------

#include <cui/panels/WindowRollHistory.h>
#include <cui/layouts/LayoutCrapsScreen.h>
#include <cui/managers/ColorManager.h>
#include <cui/core/CuiUtils.h>
#include <cui/core/CuiStructs.h>
#include <controller/CrapsReaders.h>
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
    // Rolls keep their colors, so a new format just redraws them.
    const Format fmtConfig = Ctrl::CrapsReaders::readRollHistoryFormat();

    int column = 1;
    const int rightColumn = 1 + static_cast<int>(CharLimit);

    for (const auto& r : rolls_)
    {
        const std::string text = format(fmtConfig, r);

        if (column >= rightColumn)
            break;

        const int remaining = rightColumn - column;
        const int count = std::min(remaining, static_cast<int>(text.size()));

        attrOn(r);
        mvwaddnstr(pWin_, 0, column, text.c_str(), count);
        attrOff(r);

        column += count;
    }
}

//----------------------------------------------------------------

void
WindowRollHistory::attrOn(const Roll& r)
{
    if (r.sevenOut)
    {
        wattron(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::SevenOut)));
    }
    else if (r.passLineWinner)
    {
        wattron(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::PassLineWinner)));
    }
    else if (r.point)
    {
        wattron(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::Point)));
    }
}

//----------------------------------------------------------------

void
WindowRollHistory::attrOff(const Roll& r)
{
    if (r.sevenOut)
    {
        wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::SevenOut)));
    }
    else if (r.passLineWinner)
    {
        wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::PassLineWinner)));
    }
    else if (r.point)
    {
        wattroff(pWin_, A_BOLD | COLOR_PAIR(ColorManager::instance().pair(Color::Point)));
    }
}

//----------------------------------------------------------------
//
// Formats 1 to 4, most compact first. Each ends with its separator.
//
//   1  7 8 6 10
//   2  7,8,6,10
//   3  7(34) 8(44) 6(33) 10(65)
//   4  7(3,4) 8(4,4) 6(3,3) 10(6,5)
//
std::string
WindowRollHistory::format(Format fmt, const Roll& r) const
{
    const std::string d1_s (std::to_string(r.d1));
    const std::string d2_s (std::to_string(r.d2));
    const std::string val_s(std::to_string(r.d1 + r.d2));

    switch (fmt)
    {
    case Format::Spaced:     return val_s + " ";
    case Format::Commas:     return val_s + ",";
    case Format::DiceJoined: return val_s + "(" + d1_s + d2_s + ") ";
    case Format::DiceComma:  return val_s + "(" + d1_s + "," + d2_s + ") ";
    }
    return val_s + " ";
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
WindowRollHistory::onPointEstablished(unsigned point)
{
    // Retro-actively mark the front element indicating pass line winner.
    rolls_[0].point = true;
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

