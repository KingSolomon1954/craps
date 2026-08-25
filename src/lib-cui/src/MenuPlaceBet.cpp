//----------------------------------------------------------------
//
// File: MenuPlaceBet.cpp
//
//----------------------------------------------------------------

#include <cui/MenuPlaceBet.h>
#include <cstring>
#include <cassert>
#include <cui/ConsoleView.h>

using namespace Cui;

class ConsoleManager;  // fwd
   
//----------------------------------------------------------------

MenuPlaceBet::MenuPlaceBet()
{
    createWindow();
    fillWindow();
}

//----------------------------------------------------------------

MenuPlaceBet*
MenuPlaceBet::instance()
{
    static MenuPlaceBet menu;
    return &menu;
}

//----------------------------------------------------------------

MenuPlaceBet::createWindow()
{
    using L = Layout;
    pWin_ = newwin(L::winHeight, L::winWidth, L::winStartY, L::winStartX);
    
    if (pWin_ == nullptr)
    {
        throw std::runtime_error(
            "Unable to create ncurses MenuPlaceBet window");
    }
}

//----------------------------------------------------------------

MenuPlaceBet::~MenuPlaceBet()
{
    // pWin_ is delwin() in base class
}

//----------------------------------------------------------------
//
// Fills in the window like this, just this once at init time.
// 
// Later, multiple calls to draw() just transfers the already
// filled window.
//
// 0  ┌────────────────────┐
// 1  │ Place Which Number │
// 2  ├────────────────────┤
// 3  │ [4] Place 4        │
// 4  │ [5] Place 5        │
// 5  │ [6] Place 6        │
// 6  │ [8] Place 8        │
// 7  │ [9] Place 9        │
// 8  │ [0] Place 10       │
// 9  │ [esc] Back         │
// 10 └────────────────────┘

void
MenuPlaceBet::fillWindow()
{
    using L = Layout;
    
    // Draw our border.
    box(pWin_, 0, 0);

    // Draw the horizontal separator below the title.
    mvwhline(pWin_, 2, 1, ACS_HLINE, L::winWidth - 2);
    mvwaddch(pWin_, 2, 0, ACS_LTEE);
    mvwaddch(pWin_, 2, L::winWidth - 1, ACS_RTEE);

    // Static contents. The border occupies row 0/10 and column 0/21.
    mvwaddstr(win, 1, 2, "Place Which Number");

    mvwaddstr(win, 3, 2, "[4] Place 4");
    mvwaddstr(win, 4, 2, "[5] Place 5");
    mvwaddstr(win, 5, 2, "[6] Place 6");
    mvwaddstr(win, 6, 2, "[8] Place 8");
    mvwaddstr(win, 7, 2, "[9] Place 9");
    mvwaddstr(win, 8, 2, "[0] Place 10");
    mvwaddstr(win, 9, 2, "[esc] Back");

}




#include <ncurses.h>
#include <stdexcept>

WINDOW* createPlaceWindow(int y, int x)
{

    // Draw the outer border.
    box(win, 0, 0);

    // Draw the horizontal separator below the title.
    mvwhline(win, 2, 1, ACS_HLINE, width - 2);
    mvwaddch(win, 2, 0,      ACS_LTEE);
    mvwaddch(win, 2, width - 1, ACS_RTEE);

    // Static contents. The border occupies row 0/10 and column 0/21.
    mvwaddstr(win, 1, 2, "Place Which Number");

    mvwaddstr(win, 3, 2, "[4] Place 4");
    mvwaddstr(win, 4, 2, "[5] Place 5");
    mvwaddstr(win, 5, 2, "[6] Place 6");
    mvwaddstr(win, 6, 2, "[8] Place 8");
    mvwaddstr(win, 7, 2, "[9] Place 9");
    mvwaddstr(win, 8, 2, "[0] Place 10");
    mvwaddstr(win, 9, 2, "[esc] Back");

    // Display the window.
    wrefresh(win);

    return win;
}




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
