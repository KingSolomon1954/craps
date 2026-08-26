//----------------------------------------------------------------
//
// File: MenuOdds.cpp
//
//----------------------------------------------------------------

#include <cui/MenuOdds.h>
#include <algorithm>
#include <controller/CrapsReaders.h>
#include <cui/ConsoleView.h>
#include <gen/MoneyUtils.h>
#include <gen/ErrorPass.h>
#include <cstring>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

MenuOdds::MenuOdds()
    : MenuBase(1, 1)  // Placeholder, resized in drawMenu()
{
    visible_ = true;
}

//----------------------------------------------------------------

MenuOdds&
MenuOdds::instance()
{
    static MenuOdds menu;
    return menu;
}

//----------------------------------------------------------------

MenuOdds::~MenuOdds()
{
}

//----------------------------------------------------------------
//
// Draw the menu
//
// Overrides menu base class.
//
// Dynamically builds the menu prompts from list of player's odds bets.
// Shows only active odds capable bets (sorted most recent first).
// 
// Looks something like this:
//     
// 0   ┌─────────────────────────────────┐
// 1   │ Which Odds Bet                  │
// 2   ├─────────────────────────────────┤
// 3   │ [1] Pass Line 6 ($200, $0)      │
// 4   │ [2] Come Bet 8 ($1,000, $3,000) │
// 5   │ [3] Don't Pass 10 ($500, $500)  │
// 6   │ [esc] Back                      │
// 7   └─────────────────────────────────┘
// 
void
MenuOdds::draw()
{
    gatherEntries();
    formatEntries();
    windowResize();
        
    werase(pWin_);
    drawBorders();
    drawStaticContent();
    populate();
    CuiUtils::transfer(pWin_);
}

see example dynamic menu activate/deactivate at bottom of file

//----------------------------------------------------------------





    
    assert(w_ && "MenuOdds: WINDOW not initialized");

    const std::string title{"Make Odds Bet"};
    const std::string prompt{"Select:"};

    buildOddsBetEntries(); // Prepare bets_ (only odds-capable bets)

    std::vector<std::string> entries;  // Build menu entries (strings only)
    buildMenuEntries(entries);

    int longest = getLongestEntry(entries, title, prompt);

    // Compute window size and clamp to reasonable limits (max 80 cols)
    int screenRows, screenCols;
    getmaxyx(stdscr, screenRows, screenCols);

    const int maxInnerWidth = std::min(80, screenCols - 4); // leave margin on sides
    int innerWidth = std::min(maxInnerWidth, longest + 4);  // padding inside
    int boxWidth = innerWidth + 2; // include borders

    // Rows: title(1) + blank(1) + entries(n) + esc(1) + blank(1) + prompt(1)
    int innerHeight = 1 + 1 + static_cast<int>(entries.size()) + 1 + 1 + 1;
    int boxHeight = innerHeight + 2; // include top/bottom borders

    // Ensure we fit vertically
    if (boxHeight > screenRows - 2)
    {
        // clamp height and trim entries if necessary
        boxHeight = screenRows - 2;
        innerHeight = boxHeight - 2;
        // keep top title + prompt + ESC, trim entries to fit
        int minNeeded = 1 + 1 + 1 + 1 + 1; // title + blank + esc + blank + prompt
        int allowedEntries = std::max(0, innerHeight - minNeeded);
        if (allowedEntries < static_cast<int>(entries.size()))
            entries.resize(static_cast<size_t>(allowedEntries));
    }

    // Center window
    int startY = (screenRows - boxHeight) / 2;
    int startX = (screenCols - boxWidth) / 2;

    // Recreate window sized to content
    if (w_) delwin(w_);
    w_ = newwin(boxHeight, boxWidth, startY, startX);

    // Clear, draw box, and title
    werase(w_);
    box(w_, 0, 0);

    int row = 1;
    int col = 2; // left padding inside box

    // Title (left aligned)
    mvwprintw(w_, row, col, "%s", title.c_str());
    row += 2; // title + blank line

    // Render entries
    renderMenuEntries(entries, row);

    // After entries, ensure we move row past them
    row += static_cast<int>(entries.size());

    // ESC line
    mvwprintw(w_, row++, col, "ESC - cancel");

    // blank line
    row++;

    // Prompt centered horizontally inside innerWidth
    int promptX = std::max(1, (innerWidth - static_cast<int>(prompt.size())) / 2);
    mvwprintw(w_, row++, promptX, "%s", prompt.c_str());

    // Done: schedule window for refresh
    wnoutrefresh(w_);
}

//----------------------------------------------------------------
//
// Create a vector of bets that are odds capable.
//
void
MenuOdds::buildOddsBetEntries()
{
    Gen::ErrorPass  ep;
    Craps::PlayerId playerId;

    // Grab playerId
    auto rc = Ctrl::CrapsReaders::getUserPlayer(playerId, ep);
    assert(rc == Gen::ReturnCode::Success);

    // Get player's odds capable bets
    std::vector<Craps::BetId> betIds;
    rc = Ctrl::CrapsReaders::readPlayerGetOddsBets(playerId, betIds, ep); 
    assert(rc == Gen::ReturnCode::Success);

    populateBets(betIds);  // Populate our bets_ struct
}

//----------------------------------------------------------------

void
MenuOdds::populateBets(std::vector<Craps::BetId>& betIds)
{
    bets_.clear();
    Gen::ErrorPass ep;
    
    for (auto id : betIds)
    {
        BetEntry be;

        be.betId = id;
        Ctrl::CrapsReaders::readBetPlayerId      (id, be.playerId,          ep);
        Ctrl::CrapsReaders::readBetName          (id, be.betName,           ep);
        Ctrl::CrapsReaders::readBetPivot         (id, be.pivot,             ep);
        Ctrl::CrapsReaders::readBetContractAmount(id, be.contractAmount,    ep);
        Ctrl::CrapsReaders::readBetOddsAmount    (id, be.currentOddsAmount, ep);
        Ctrl::CrapsReaders::readBetWhenCreated   (id, be.whenCreated,       ep);
        
        bets_.push_back(be);
    }
    sortBetsByCreated();
}

//----------------------------------------------------------------
//
// Create a vector of bets that are odds capable.
//
void
MenuOdds::buildMenuEntries(std::vector<std::string>& entries)
{
    entries.clear();

    // First pass: build prefixes and amounts separately
    std::vector<std::string> prefixes;
    std::vector<std::string> amounts;

    for (size_t i = 0; i < bets_.size(); ++i)
    {
        char key = betKeyFromIndex(i);
        if (key == '\0') break; // out of supported range

        auto& b = bets_[i];
        std::string name       = EnumBetName::toString(b.betName);
        std::string pivot      = std::to_string(b.pivot);
        std::string amount     = Gen::MoneyUtils::toStringNoCommas(b.contractAmount);
        std::string oddsAmount = Gen::MoneyUtils::toStringNoCommas(b.currentOddsAmount);

        // Build everything before the amounts
        std::string prefix = std::string(1, key) + " - " + name + " " + pivot;

        // Build just the amounts portion
        std::string amt = "(" + amount + ", " + oddsAmount + ")";

        prefixes.push_back(prefix);
        amounts.push_back(amt);
    }

    // Find the longest prefix
    size_t maxPrefixLen = 0;
    for (auto& p : prefixes)
        maxPrefixLen = std::max(maxPrefixLen, p.size());

    // Combine into final vertically aligned entries
    for (size_t i = 0; i < prefixes.size(); ++i)
    {
        std::string line = prefixes[i];
        // pad with spaces so all amounts start at same column
        if (line.size() < maxPrefixLen)
            line.append(maxPrefixLen - line.size(), ' ');

        line += " " + amounts[i];
        entries.push_back(line);
    }
}

//----------------------------------------------------------------

void
MenuOdds::renderMenuEntries(const std::vector<std::string>& entries,
                            int startRow)
{
    int row = startRow;
    int col = 2; // left padding inside box

    for (const auto &e : entries)
    {
        mvwprintw(w_, row++, col, "%s", e.c_str());
    }
}

//----------------------------------------------------------------

int
MenuOdds::getLongestEntry(const std::vector<std::string>& entries,
                          const std::string& title,
                          const std::string&  prompt) const
{
    int longest = static_cast<int>(title.size());
    longest = std::max(longest, static_cast<int>(prompt.size()));
    for (const auto &e : entries)
    {
        longest = std::max(longest, static_cast<int>(e.size()));
    }
    return longest;
}

//----------------------------------------------------------------
//
// Override menu base class
//
void
MenuOdds::handleMenuKey(int ch)
{
    int idx = betIndexFromKey(ch, bets_.size());
    if (idx >= 0)
    {
        auto& bet = bets_[static_cast<size_t>(idx)];
        doSelection(bet);
        return;
    }
    beep();
}

//----------------------------------------------------------------

void
MenuOdds::sortBetsByCreated()
{
    std::sort(bets_.begin(), bets_.end(),
        [](const BetEntry& a, const BetEntry& b)
        {
            return a.whenCreated > b.whenCreated;  // newest first
        });
}

//----------------------------------------------------------------
//
// Map input key to bet index
//
int
MenuOdds::betIndexFromKey(int ch, size_t betCount)
{
    // '1'..'9'
    if (ch >= '1' && ch <= '9')
    {
        int idx = ch - '1';  // '1' → 0
        if (idx < static_cast<int>(betCount))
            return idx;
    }

    // 'a'..'c' (case-insensitive)
    if (ch >= 'a' && ch <= 'c')
    {
        int idx = 9 + (ch - 'a');  // 'a' → 9
        if (idx < static_cast<int>(betCount))
            return idx;
    }
    if (ch >= 'A' && ch <= 'C')
    {
        int idx = 9 + (ch - 'A');  // 'A' → 9
        if (idx < static_cast<int>(betCount))
            return idx;
    }

    return -1; // Invalid
}

//----------------------------------------------------------------
//
// Map bet index to display key
//
char
MenuOdds::betKeyFromIndex(size_t idx)
{
    if (idx < 9)
        return static_cast<char>('1' + idx);  // 0→'1', 8→'9'
    else if (idx < 12)
        return static_cast<char>('a' + (idx - 9));  // 9→'a', 11→'c'

    return '\0'; // Out of supported range
}

//----------------------------------------------------------------
//
// User selected an entry from the menu
//
void
MenuOdds::doSelection(BetEntry& bet)
{
    // Load results struct for the selected bet
    playerId_          = bet.playerId;
    betName_           = bet.betName;
    betId_             = bet.betId;
    pivot_             = bet.pivot;
    contractAmount_    = bet.contractAmount;    
    currentOddsAmount_ = bet.currentOddsAmount;    
    view_.popScreen();
}    

//----------------------------------------------------------------

MenuOdds::Results
MenuOdds::getResults() const
{
    Results results;
    results.canceled          = isCanceled_;
    results.playerId          = playerId_;         
    results.betName           = betName_;          
    results.betId             = betId_;            
    results.pivot             = pivot_;            
    results.contractAmount    = contractAmount_;   
    results.currentOddsAmount = currentOddsAmount_;
    return results;
}

//----------------------------------------------------------------

void
MenuOdds::clearState()
{
    playerId_.clear();
    betName_           = BetName::Invalid;
    betId_             = 0;
    pivot_             = 0;
    contractAmount_    = 0;
    currentOddsAmount_ = 0;
    isCanceled_        = false;  // base class
}    

//----------------------------------------------------------------


#if 0

The usual pattern is:

    Calculate the required height and width.
    Create the window if it does not exist.
    Resize and move it if it already exists.
    Clear and redraw its contents.

-------------------
Activate/Deactivate
-------------------

class NcursesMenu
{
public:
    NcursesMenu(int y, int x)
        : y_(y), x_(x)
    {
    }

    ~NcursesMenu()
    {
        destroyWindow();
    }

    void activate(const std::vector<std::string>& entries)
    {
        entries_ = entries;

        const auto [height, width] = calculateSize();

        if (window_ == nullptr) {
            window_ = newwin(height, width, y_, x_);

            if (window_ == nullptr) {
                throw std::runtime_error("Unable to create menu window");
            }
        }
        else {
            wresize(window_, height, width);
            mvwin(window_, y_, x_);
        }

        draw();
        active_ = true;
    }

    void deactivate()
    {
        if (window_ == nullptr || !active_) {
            return;
        }

        // Erase the menu's current contents from the virtual screen.
        werase(window_);
        wnoutrefresh(window_);

        // Ensure the underlying window redraws the area previously covered
        // by the menu. This assumes backgroundWindow_ is set.
        if (backgroundWindow_ != nullptr) {
            touchwin(backgroundWindow_);
            wnoutrefresh(backgroundWindow_);
        }

        doupdate();

        active_ = false;
    }

    void setBackgroundWindow(WINDOW* backgroundWindow)
    {
        backgroundWindow_ = backgroundWindow;
    }

    int getch()
    {
        if (window_ == nullptr || !active_) {
            return ERR;
        }

        return wgetch(window_);
    }

private:
    std::pair<int, int> calculateSize() const
    {
        constexpr int horizontalPadding = 4; // two spaces on each side
        constexpr int borderRows = 2;

        int width = 0;

        for (const auto& entry : entries_) {
            width = std::max(
                width,
                static_cast<int>(entry.size()));
        }

        // Add room for the two border columns and horizontal padding.
        width += horizontalPadding + 2;

        // At least two columns inside the border.
        width = std::max(width, 4);

        const int height =
            static_cast<int>(entries_.size()) + borderRows;

        return {height, width};
    }

    void draw()
    {
        werase(window_);
        box(window_, 0, 0);

        for (std::size_t row = 0; row < entries_.size(); ++row) {
            mvwaddnstr(
                window_,
                static_cast<int>(row) + 1,
                2,
                entries_[row].c_str(),
                getmaxx(window_) - 3);
        }

        wnoutrefresh(window_);
        doupdate();
    }

    void destroyWindow()
    {
        if (window_ != nullptr) {
            delwin(window_);
            window_ = nullptr;
        }
    }

private:
    WINDOW* window_ = nullptr;
    WINDOW* backgroundWindow_ = nullptr;

    std::vector<std::string> entries_;

    int y_;
    int x_;
    bool active_ = false;
};


* Use it like this:

NcursesMenu menu(5, 10);
menu.setBackgroundWindow(mainWindow);

menu.activate({
    "[4] Place 4",
    "[5] Place 5",
    "[6] Place 6",
    "[8] Place 8",
    "[9] Place 9",
    "[0] Place 10",
    "[esc] Back"
});

// Read input while active.
int ch = menu.getch();

menu.deactivate();

#endif
