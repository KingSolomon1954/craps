#include <algorithm>
#include <iostream>
#include <ncurses.h>
#include <optional>
#include <string>
#include <vector>

//----------------------------------------------------------------

struct BetInfo
{
    int betId;
    std::string betName;  // this is an Enum in craps game
    int pivot;
    int contractAmount;
    int currentOddsAmount;
    int whenCreated;  // Using integer for time placeholder to simplify
};

using Bets = std::vector<BetInfo>;

Bets fillOddsBets1()
{
    return {
        {3232, "PassLine",  6, 100,   0, 17},
        {4111, "DontCome", 10, 100,   0, 19},
        {687,  "Come",      4,  50, 100, 18},
    };
}

Bets fillOddsBets2()
{
    return {
        {6659, "PassLine", 6,  100,   0, 16},
        {8022, "Come",     4,   50, 100, 17},
        {7977, "DontCome", 10, 100,   0, 20},
        {6868, "DontCome", 8,    5,   0, 15},
        {1412, "DontCome", 6,    5,   0, 18},
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
        buildMenuEntries();
        windowResize();
        
        werase(pWin_);
        drawBorders();
        drawStaticContent();
        populate();
        wnoutrefresh(pWin_);
        doupdate();
        
// getch();    
//      wrefresh(pWin_);
//      wrefresh(pWin_);
//      refresh();
    }
    
private:
    struct MenuEntry
    {
        char        hotKey;
        int         betId;
        std::string text;
    };
    using MenuEntries = std::vector<MenuEntry>;
    
    WINDOW* pWin_ = nullptr;
    int winBorderTopRow_ = 0;
    int winBorderTopCol_ = 0;
    
    Bets         activeOddsBets_;
    MenuEntries  menuEntries_;
    static constexpr std::string_view MenuHotKeys =
        "123456789abcdefghijklmnopqrstuvwxyz";

    void gatherEntries()
    {
        // builds vector of structs holding odds bets
        // calls into controller in actual game
        // get player id of interest
        // get vector of player's odds capable bets
        // Here just simulate obtaining various odds bets
        activeOddsBets_.clear();
//      activeOddsBets_ = fillOddsBets1();
        activeOddsBets_ = fillOddsBets2();
        sortByWhenCreated();
    }

    void sortByWhenCreated()
    {
        // sort the list by most recent bet first
        std::sort(activeOddsBets_.begin(), activeOddsBets_.end(),
            [](const BetInfo& a, const BetInfo& b)
            {
                return a.whenCreated < b.whenCreated;
            });
    }

    void buildMenuEntries()
    {
        menuEntries_.clear();

        for (size_t i = 0; i < activeOddsBets_.size(); ++i)
        {
            menuEntries_.push_back({
                .hotKey = indexToHotKey(i),
                .betId  = activeOddsBets_[i].betId,
                .text   = formatBet(activeOddsBets_[i])
            });
        }
    }

    std::string formatBet(const BetInfo& bet)
    {
        std::string name   = convertToLabel(bet.betName);
        std::string amt    = formatAmount(bet);
        std::string pivot  = formatPivot(bet);
        std::string hotKey = formatHotKey("?");
        return hotKey + " " + name + " " + pivot + " " + amt;
    }

    std::string formatAmount(const BetInfo& bet)
    {
        std::string s = "($" + std::to_string(bet.contractAmount);
        s += ", $" + std::to_string(bet.currentOddsAmount) + ")";
        return s;
    }

    std::string formatPivot(const BetInfo& bet)
    {
        return std::to_string(bet.pivot);
    }
    
    std::string formatHotKey(std::string k)
    {
        return "[" + k + "]";
    }
    
    std::string convertToLabel(const std::string& s)
    {
        if (s == "PassLine") return "Pass Line";
        if (s == "DontCome") return "Dont Come";
        if (s == "Come")     return "Come";
        
        return "expand convertToLabel";
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
        
        int height = menuEntries_.size() + rowsAbove + rowsBelow;

        int width = 0;
        for (const auto& e : menuEntries_)
        {
            width = std::max(width, static_cast<int>(e.text.size()));
        }
        width += blankCols + borderCols;
        return {height, width};
    }

    void drawBorders()
    {
        box(pWin_, 0, 0);

        int height, width;
        getmaxyx(pWin_, height, width);

        // Draw the horizontal separator below the title.
        mvwhline(pWin_, 2, 1, ACS_HLINE, width - 2);
        mvwaddch(pWin_, 2, 0, ACS_LTEE);
        mvwaddch(pWin_, 2, width - 1, ACS_RTEE);
    }

    void drawStaticContent()
    {
        // Menu title bar
       mvwprintw(pWin_, 1, 2, "Which Odds Bet");
    }
    
    void populate()
    {
        for (size_t i = 0; i < menuEntries_.size(); ++i)
        {
            mvwaddstr(pWin_, i + 3, 2, menuEntries_[i].text.c_str());
        }
    }
    
    char indexToHotKey(size_t index)
    {
        if (index >= MenuHotKeys.size())
            throw std::out_of_range("Too many menu entries");

        return MenuHotKeys[index];
    }

    std::optional<size_t>
    hotKeyToIndex(int ch, size_t numEntries)
    {
        const auto pos = MenuHotKeys.find(static_cast<char>(ch));

        if (pos == std::string_view::npos || pos >= numEntries)
        {
            return std::nullopt;
        }

        return pos;
    }    
    
    //----------------------------------------------------------------
    //
    // Override surface base class
    //
    void handleKey(int ch)
    {
        auto it = std::find_if(
            menuEntries_.begin(),
            menuEntries_.end(),
            [ch](const MenuEntry& entry)
            {
                return entry.hotKey == ch;
            });

        if (it == menuEntries_.end())
            return;

        // TODO
        // selectBet(it->betId);
    }

};  // class DynamicMenu


//----------------------------------------------------------------

int main()
{
    initscr();
    cbreak();
    noecho();
    curs_set(0); // hide cursor
    refresh();
    
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

#if 0
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
        for (size_t i = 0; i < activeOddsBets_.size(); ++i)
        {
            auto& bet = activeOddsBets_[i];
            std::string s = convertToLabel(bet.betName) + " " +
                            std::to_string(bet.pivot);
            labels.push_back(s);
        }
    }
    
    std::string formatHotKey(?);

    void formatAmounts(StringVec& amounts)
    {
        for (size_t i = 0; i < activeOddsBets_.size(); ++i)
        {
            auto& bet = activeOddsBets__[i];
            std::string s = "($" + std::to_string(bet.contractAmount);
            s += ", $" + std::to_string(bet.currentOddsAmount) + ")";
            amounts.push_back(s);
        }
    }
    
    void formatHotKeys(StringVec& hotKeys)
    {
        for (size_t i = 0; i < activeOddsBets_.size(); ++i)
        {
            std::string s = std::string("[") + hotKeyFromIndex(i) + "]";
            hotKeys.push_back(s);
        }
    }

    void formatFinal(StringVec& hotKeys, StringVec& labels, StringVec& amounts)
    {
        menuEntries_.clear();
        for (size_t i = 0; i < labels.size(); ++i)
        {
            std::string s = hotKeys[i] + " " + labels[i] + " " + amounts[i];
            menuEntries_.push_back(s);
        }
        finalEntries_.push_back("[esc] back");
    }

    //----------------------------------------------------------------
    //
    // Turn vector index into a single digit character for the prompt
    //
    char hotKeyFromIndex(size_t idx)
    {
        // There's a max of 12 odds bets in Craps
        assert(idx <= 11);
        
        if (idx < 9)
            return static_cast<char>('1' + idx);  // 0→'1', 8→'9'
        else
            return static_cast<char>('a' + (idx - 9));  // 9→'a', 11→'c'
    }

    //----------------------------------------------------------------
    //
    // Turn single digit hotKey into vector index
    //
    size_t hotKeyToIndex(int ch)
    {
        if ch 
        // '1'..'9'
        if (ch >= '1' && ch <= '9')
        {
            return ch - '1';  // '1' → 0
        }

        // 'a'..'c' (case-insensitive)
        if (ch >= 'a' && ch <= 'c')
        {
            return '9' + (ch - 'a');  // 'a' → 9
        }
        return -1; // Invalid
    }

#endif
