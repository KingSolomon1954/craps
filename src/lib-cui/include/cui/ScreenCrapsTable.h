//----------------------------------------------------------------
//
// File: ScreenCrapsTable.h
//
//----------------------------------------------------------------

#pragma once

#include <vector>
#include <ncurses.h>
#include <cui/Screen.h>
#include <craps/CrapsTypes.h>

namespace Cui {

class ScreenCrapsTable : public Screen
{
public:
    ScreenCrapsTable(ConsoleView& view);
   ~ScreenCrapsTable() override;
    
    void draw()            override;
    void onAttach()        override;
    void handleKey(int ch) override;

private:
    struct Windows
    {
        WINDOW *header  = nullptr, *animation   = nullptr,
               *history = nullptr, *houseBrief  = nullptr,
               *table   = nullptr, *playerBrief = nullptr,
               *message = nullptr;
    };

    enum class Menus
    {
        Betting,
        Stats
    };

    enum class TableView
    {
        AllPlayers,
        OnePlayer
    };

    enum class Column
    {
        Left,
        Right
    };

    struct Layout
    {
        // Overall dimensions
        static constexpr int totalRows = 38;
        static constexpr int totalCols = 100;

        // Left/right split
        static constexpr int leftW  = 70;
        static constexpr int rightW = totalCols - leftW;

        // Row positions of horizontal dividers (y-coords)
        static constexpr int rowHistory = 2;
        static constexpr int rowTable   = 4;
        static constexpr int rowMessage = 23;
        static constexpr int rowPlayer  = 29;

        static constexpr int animationRows = 24;
        static constexpr int animationCols = 30;
        static constexpr int diceHeight = 5;
        static constexpr int diceWidth = 7;
    };

    struct LayoutAllPlayer
    {
        static constexpr int col1 = 0;
        static constexpr int col2 = 25;
        static constexpr int col3 = 48;
    };
    
    struct LayoutOnePlayer
    {
        static constexpr int col1 = 0;
        static constexpr int col2 = 19;
        static constexpr int col3 = 29;
        static constexpr int col4 = 39;
        static constexpr int col5 = 49;
        static constexpr int col6 = 59;
        static constexpr int lineBetSplitCol = 59;

        static constexpr int rowField    = 14;
        static constexpr int rowCraps    = 18;
        static constexpr int rowLineBets = 21;
    };
    
    // Order doesn't matter    
    Craps::PlayerId userPlayerId_;
    Craps::TableId  tableId_;
    Windows w_;
    std::string lineBuffer_;
    Menus activeMenu_ = Menus::Betting;
    TableView tableView_ = TableView::AllPlayers;
    size_t onePlayerIndex_ = 0;  // For cycling thru players
    std::vector<Craps::PlayerId> playerIds_;
    
    // Input handling
    void handleLineInput  (int ch);
    void handleMenuInput  (int ch);
    void handleAmountInput(int ch);
    void menuInputBetting (int ch);
    void menuInputStats   (int ch);
    void setMenuInputMode();
    void setLineInputMode();
    void cycleTableView();

    // Drawing screen borders/frame
    void drawFrame();
    void drawConnectorsAllPlayers();
    void drawConnectorsOnePlayer();
    void eraseConnectorsAllPlayers();
    void eraseConnectorsOnePlayer();
    
    // Drawing table subwindow
    void drawHeader();
    void drawHistory();
    void drawTable();
    void drawMessages();
    void drawAnimation();
    void drawHouseBrief();
    void drawPlayerBrief();
    void drawTableAllPlayers();
    void drawTableOnePlayer();
    Craps::PlayerId getPlayerAt(size_t index);

    // Draw Amount subwindow
    // TODO

    void setupQuickBet();
    void setupAutoFill();
    void processAmountBuffer();
    void processLineBuffer();
    void createSubwindows();
    WINDOW* makeSubwin(int topDivider,
                       int bottomDivider,
                       int innerW,
                       Column col);
    void drawInputPrompt();
};

// TODO: subscribe to players leaving/joining table
// TODO: update PlayerList upon notification

//----------------------------------------------------------------

} // namespace Cui

//----------------------------------------------------------------
