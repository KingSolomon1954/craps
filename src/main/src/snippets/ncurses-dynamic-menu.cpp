#include <algorithm>
#include <iostream>
#include <ncurses.h>
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
        gatherBets();
        buildMenuEntries();
        windowResize();

        werase(pWin_);
        drawBorders();
        drawStaticContent();
        populate();
        wnoutrefresh(pWin_);
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

    void gatherBets()
    {
        // builds vector of structs holding active odds bets
        // calls into controller in actual game
        // gets player id of interest
        // gets vector of player's odds capable bets
        // Here just simulate obtaining various odds bets
        // from hard coded structs
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
        buildMenuBets();
        addQuitEntry();
    }

    void buildMenuBets()
    {
        for (size_t i = 0; i < activeOddsBets_.size(); ++i)
        {
            const char hotKey = indexToHotKey(i);
            
            menuEntries_.push_back({
                .hotKey = hotKey,
                .betId  = activeOddsBets_[i].betId,
                .text   = formatHotKey(hotKey) + " " +
                          formatBet(activeOddsBets_[i])
            });
        }
    }

    char indexToHotKey(size_t index)
    {
        if (index >= MenuHotKeys.size())
            throw std::out_of_range("Too many menu entries");

        return MenuHotKeys[index];
    }

    void addQuitEntry()
    {
        menuEntries_.push_back({.hotKey = 27, .betId = 0, .text = "[esc] Back"});
    }
        
    std::string formatBet(const BetInfo& bet)
    {
        std::string name   = convertToLabel(bet.betName);
        std::string amt    = formatAmount(bet);
        std::string pivot  = formatPivot(bet);
        return name + " " + pivot + " " + amt;
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
    
    std::string formatHotKey(const char hotKey)
    {
        return "[" + std::string(1, hotKey) + "]";
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

        // Horizontal separator below the title.
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
    
    //----------------------------------------------------------------
    //
    // Override surface base class
    //
    void handleKey(int ch)
    {
        if (ch == 27)  // escape
        {
            quit();
            return;
        }
                          
        auto it = std::find_if(
            menuEntries_.begin(),
            menuEntries_.end(),
            [ch](const MenuEntry& entry)
            {
                return entry.hotKey == ch;
            });

        if (it == menuEntries_.end()) return;

        processSelection(it->betId);
    }

    //----------------------------------------------------------------

    void quit()
    {
        // Set our own state in base class to reflect cancel.
        // Also informs parent surfaces of the state of operation.
        // In turn, parent menus can decide if they are skipped
        // when unwinding the menu stack.
        //
        // setOperationResult(OperationResult::cancel);
        // ConsoleManager::popSurfaces();
    }

    //----------------------------------------------------------------

    void processSelection(int betId)
    {
        const BetInfo& bet = findBet(betId);
        populateCarrier(bet);
        prepDialogAmount(bet);
        activateDialogAmount();
    }

    //----------------------------------------------------------------

    BetInfo& findBet(int betId)
    {
        auto it = std::find_if(
            activeOddsBets_.begin(),
            activeOddsBets_.end(),
            [betId](const BetInfo& bet)
            {
                return bet.betId == betId;
            });

        if (it == activeOddsBets_.end())
        {
            throw std::runtime_error(
                "Unable to match betId. Programmer error.");
        }
    }

    //----------------------------------------------------------------

    void populateCarrier(const BetInfo& bet)
    {
        // CarrierBet::clear();
        // CarrierBet::setBetType(bet.betName);
        // CarrierBet::setPivot(bet.pivot);
    }

    //----------------------------------------------------------------

    void prepDialogAmount(const BetInfo& bet)
    {
        // DialogBetAmount::setPrompt("Odds on %s", pivot_);
        // auto amount = getAutoFillAmount(OddsBet, pivot_);
        // DialogBetAmount::preFill(amount);
    }

    //----------------------------------------------------------------

    void activateDialogAmount()
    {
        // ConsoleManager::pushSurface(AmountDialog);
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
