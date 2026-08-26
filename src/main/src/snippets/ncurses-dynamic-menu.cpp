#include <algorithm>
#include <iostream>
#include <ncurses.h>
#include <string>
#include <vector>

//----------------------------------------------------------------

struct BetInfo
{
    std::string betName;
    int pivot;
    int contractAmount;
    int currentOddsAmount;
    int whenCreated;  // Using integer for time to simplify
};

using BetsInfoList = std::vector<BetInfo>;
using StringVec = std::vector<std::string>;

BetsInfoList fillOddsBets1()
{
    return {
        {"PassLine",  6, 100,   0, 19},
        {"Come",      4,  50, 100, 18},
        {"DontCome", 10, 100,   0, 17},
    };
}

BetsInfoList fillOddsBets2()
{
    return {
        {"PassLine", 6,  100,   0, 16},
        {"Come",     4,   50, 100, 17},
        {"DontCome", 10, 100,   0, 20},
        {"DontCome", 8,    5,   0, 15},
        {"DontCome", 6,    5,   0, 18},
    };
}

class DynamicMenu
{
public:
    DynamicMenu(int y, int x)
        : winBorderTopCol_(y)
        , winBorderTopRow_(x)
    {
    }

    ~DynamicMenu()
    {
        if (pWin_ != nullptr) delwin(pWin_);
    }

    void draw()
    {
        gatherEntries();
        formatEntries();
        windowResize();
        
        werase(pWin_);
        drawBorders();
        drawStaticContent();
        populate();
        wnoutrefresh(stdscr);
        wnoutrefresh(pWin_);
        doupdate();
        
// getch();    
//      wrefresh(pWin_);
//      wrefresh(pWin_);
//      refresh();
    }
    
private:
    WINDOW* pWin_ = nullptr;
    int winBorderTopRow_ = 0;
    int winBorderTopCol_ = 0;
    
    BetsInfoList betsInfoList_;
    StringVec    finalEntries_;

    void gatherEntries()
    {
        // builds vector of structs holding odds bets
        // get player id of interest
        // get vector of player's odds capable bets
        // Here we simulate obtaining various odds bets
        betsInfoList_.clear();
        betsInfoList_ = fillOddsBets1();
        // sort the list by most recent bet first
        sortByWhenCreated();
    }

    void sortByWhenCreated()
    {
        std::sort(betsInfoList_.begin(), betsInfoList_.end(),
            [](const BetInfo& a, const BetInfo& b)
            {
                return a.whenCreated > b.whenCreated;  // newest first
            });
    }
    
    // builds vector of formatted strings for menu display
    void formatEntries()
    {
        StringVec labels;
        StringVec amounts;
        StringVec hotKeys;
        
        formatLabels(labels);
        formatAmounts(amounts);
        formatHotKeys(hotKeys);
        formatFinal(hotKeys, labels, amounts);
    }

    void formatLabels(StringVec& labels)
    {
        for (size_t i = 0; i < betsInfoList_.size(); ++i)
        {
            auto& info = betsInfoList_[i];
            std::string s = convertToLabel(info.betName) + " " +
                            std::to_string(info.pivot);
            labels.push_back(s);
        }
    }
    
    void formatAmounts(StringVec& amounts)
    {
        for (size_t i = 0; i < betsInfoList_.size(); ++i)
        {
            auto& info = betsInfoList_[i];
            std::string s = "($" + std::to_string(info.contractAmount);
            s += ", $" + std::to_string(info.currentOddsAmount) + ")";
            amounts.push_back(s);
        }
    }
    
    void formatHotKeys(StringVec& hotKeys)
    {
        for (size_t i = 0; i < betsInfoList_.size(); ++i)
        {
            std::string s = std::string("[") + hotKeyFromIndex(i) + "]";
            hotKeys.push_back(s);
        }
    }

    void formatFinal(StringVec& hotKeys, StringVec& labels, StringVec& amounts)
    {
        finalEntries_.clear();
        for (size_t i = 0; i < labels.size(); ++i)
        {
            std::string s = hotKeys[i] + " " + labels[i] + " " + amounts[i];
            finalEntries_.push_back(s);
        }
        finalEntries_.push_back("[esc] back");
    }

    std::string convertToLabel(const std::string& s)
    {
        if (s == "PassLine") return "Pass Line";
        if (s == "DontCome") return "Dont Come";
        if (s == "Come")     return "Come";
        
        return "expand convertToLabel";
    }
    
    //----------------------------------------------------------------
    //
    // Turn vector index into a single digit character for the prompt
    //
    char hotKeyFromIndex(size_t idx)
    {
        // There's a max of 12 odds bets in Craps
        if (idx < 9)
            return static_cast<char>('1' + idx);  // 0→'1', 8→'9'
        else
            return static_cast<char>('a' + (idx - 9));  // 9→'a', 11→'c'
    }

    void windowResize()
    {
        const auto [height, width] = calcSize();

        std::cout << "height:" << height << "width:" << width << std::endl;
        
        if (pWin_ == nullptr)
        {
            pWin_ = newwin(height, width, winBorderTopCol_, winBorderTopRow_);
            if (pWin_ == nullptr)
            {
                throw std::runtime_error("Unable to create menu window");
            }
        }
        else
        {
            wresize(pWin_, height, width);
            mvwin(pWin_, winBorderTopCol_, winBorderTopRow_);
        }
    }

    std::pair<int, int> calcSize() const
    {
        constexpr int rowsAbove  = 3; // Top border + title area
        constexpr int rowsBelow  = 1; // bottom border
        constexpr int borderCols = 2; // 2 vertical borders
        constexpr int blankCols  = 2; // 2 empty columns adjacent ot borders
        
        int height = finalEntries_.size() + rowsAbove + rowsBelow;

        int width = 0;
        for (const auto& e : finalEntries_)
        {
            width = std::max(width, static_cast<int>(e.size()));
        }
        width += blankCols + borderCols;
        return {height, width};
    }

    void drawBorders()
    {
        box(pWin_, 0, 0);
    }

    void drawStaticContent()
    {
        // Put the title bar 
    }
    
    void populate()
    {
    }
    

};  // class DynamicMenu


//----------------------------------------------------------------

int main()
{
    initscr();
    cbreak();
    noecho();
    curs_set(0); // hide cursor

// getch();    
    DynamicMenu dm(10,20);
    dm.draw();

    mvprintw(30, 30, "Press any key to exit...");
    doupdate();

getch();

    curs_set(1); // cursor back on
    endwin();
    return 0;
}

//----------------------------------------------------------------







#if 0

    // Test writing into each window
    mvwprintw(w.header,  0, 0, "Header text here");
    mvwprintw(w.history, 0, 0, "History entry");
    mvwprintw(w.table,   0, 0, "Table content...");
    mvwprintw(w.message, 0, 0, "Messages go here");

    mvwprintw(w.animation, 0, 0, "Animation area");
    mvwprintw(w.house,     0, 0, "House: $1000");
    mvwprintw(w.player,    0, 0, "Player: $500");
    mvwprintw(w.command,   0, 0, "Command>");

#endif













    
/*

0  ┌─[Header]────────────────────────────────────────────────────────────┬─[Animation]────────────────┐
1  │Header text here                                                     │Animation area              │
2  ├─[History]───────────────────────────────────────────────────────────┤                            │
3  │History entry                                                        │                            │
4  ├─[Table]─────────────────────────────────────────────────────────────┤                            │
5  │Table content...                                                     │                            │
6  │                                                                     │                            │
7  │                                                                     │                            │
8  │                                                                     │                            │
9  │                                                                     │                            │
10 │                                                                     │                            │
11 │                                                                     │                            │
12 │                                                                     │                            │
13 │                                                                     │                            │
14 │                                                                     │                            │
15 │                                                                     │                            │
16 │                                                                     │                            │
17 │                                                                     │                            │
18 │                                                                     │                            │
19 │                                                                     │                            │
20 │                                                                     │                            │
21 │                                                                     │                            │
22 │                                                                     │                            │
23 ├─[Message]───────────────────────────────────────────────────────────┼─[House]────────────────────┤
24 │Messages go here                                                     │House: $1000                │25 │                                                                     │                            │
26 │                                                                     │                            │
27 │                                                                     │                            │
28 │                                                                     ├─[Player]───────────────────┤
29 │                                                                     │Player: $500                │
30 │                                                                     │                            │
31 │                                                                     │                            │
32 │                                                                     │                            │
33 │                                                                     │                            │
34 │                                                                     │                            │
35 │                                                                     ├─[Command]──────────────────┤
36 │                                                                     │Command>                    │
37 └─────────────────────────────────────────────────────────────────────┴────────────────────────────┘
38  Press any key to exit...

*/
