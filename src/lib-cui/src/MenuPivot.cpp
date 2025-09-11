//----------------------------------------------------------------
//
// File: MenuPivot.cpp
//
//----------------------------------------------------------------

#include <cui/MenuPivot.h>
#include <cstring>
#include <cassert>
#include <cui/ConsoleView.h>

using namespace Cui;

//----------------------------------------------------------------

MenuPivot::MenuPivot(ConsoleView& view)
    : MenuBase(view, 1, 1)  // Placeholder, resized in setMessage()
{
}

//----------------------------------------------------------------

MenuPivot::~MenuPivot()
{
}

//----------------------------------------------------------------

void
MenuPivot::setUpFor(BetName betName)
{
    assert(betName == BetName::Place || betName == BetName::Hardway);
    
    clearState();
    betName_ = betName;
    if (betName_ == BetName::Place)
    {
        title_ = "Place which number?";
    }
    else
    {
        title_ = "Hardway bet which number?";
    }
}

//----------------------------------------------------------------
//
// Draw the menu prompts
//
// Overrides menu base class.
//
// Looks something like this:
//     
//     +-----------------------+
//     | Hardway which number? |
//     |                       |
//     | 4   - Hardway         |
//     | 6   - Hardway         |
//     | 8   - Hardway         |
//     | 10  - Hardway         |
//     | ESC - cancel          |
//     |                       |
//     | Number:               |
//     +-----------------------+
// 
//     +---------------------+
//     | Place which number? |
//     |                     |
//     | 4   - Place         |
//     | 5   - Place         |
//     | 6   - Place         |
//     | 8   - Place         |
//     | 9   - Place         |
//     | 10  - Place         |
//     | ESC - cancel        |
//     |                     |
//     | Number:             |
//     +---------------------+
//
void
MenuPivot::drawMenu()
{
    assert(w_ && "MenuPivot: WINDOW not initialized");

    // Determine number of choices
    size_t numChoices = (betName_ == BetName::Hardway) ? 4 : 6;

    // Compute box size
    int boxWidth  = 28;  // fixed width (enough for text + padding)
    int boxHeight = static_cast<int>(numChoices) + 7;

    // Clamp within screen
    int screenRows, screenCols;
    getmaxyx(stdscr, screenRows, screenCols);
    boxWidth  = std::min(boxWidth, 80);
    boxHeight = std::min(boxHeight, screenRows - 2);

    // Center
    int startY = (screenRows - boxHeight) / 2;
    int startX = (screenCols - boxWidth) / 2;

    // Recreate window
    if (w_) delwin(w_);
    w_ = newwin(boxHeight, boxWidth, startY, startX);

    // Draw border
    box(w_, 0, 0);

    // Print the title centered on row 1
    int titleX = (boxWidth - static_cast<int>(title_.size())) / 2;
    mvwprintw(w_, 1, std::max(1, titleX), "%s", title_.c_str());

    // Leave a blank line under the title
    int row = 3;

    if (betName_ == BetName::Place)
    {
        mvwprintw(w_, row++, 2, "4   - Place");
        mvwprintw(w_, row++, 2, "5   - Place");
        mvwprintw(w_, row++, 2, "6   - Place");
        mvwprintw(w_, row++, 2, "8   - Place");
        mvwprintw(w_, row++, 2, "9   - Place");
        mvwprintw(w_, row++, 2, "10  - Place");
    }
    else if (betName_ == BetName::Hardway)
    {
        mvwprintw(w_, row++, 2, "4   - Hardway");
        mvwprintw(w_, row++, 2, "6   - Hardway");
        mvwprintw(w_, row++, 2, "8   - Hardway");
        mvwprintw(w_, row++, 2, "10  - Hardway");
    }

    // ESC cancel option
    mvwprintw(w_, row++, 2, "ESC - cancel");

    // Blank spacer line
    row++;

    // Prompt near the bottom
    const char* prompt = "Number:";
    int promptX = (boxWidth - static_cast<int>(strlen(prompt))) / 2;
    mvwprintw(w_, row++, std::max(1, promptX), "%s", prompt);

    // Done, refresh
    wnoutrefresh(w_);
}

//----------------------------------------------------------------
//
// Override menu base class
//
void
MenuPivot::handleMenuKey(int ch)
{
    switch (ch)
    {
    case '4': doPivot(4);  break;
    case '5': doPivot(5);  break;
    case '6': doPivot(6);  break;
    case '8': doPivot(8);  break;
    case '9': doPivot(9);  break;
    case '1': doPivot(10); break;
    }
}

//----------------------------------------------------------------

void
MenuPivot::doPivot(int num)
{
    if (betName_ == BetName::Hardway && (num == 5 || num == 9))
    {
        return;  // Ignore
    }
    pivot_ = num;
    view_.popScreen();
}    

//----------------------------------------------------------------

MenuPivot::Results
MenuPivot::getResults() const
{
    Results results;
    results.canceled = isCanceled_;
    results.pivot    = pivot_;
    return results;
}

//----------------------------------------------------------------

void
MenuPivot::clearState()
{
    title_.clear();
    pivot_      = 0;
    betName_    = BetName::Invalid;
    isCanceled_ = false;  // base class
}    

//----------------------------------------------------------------
