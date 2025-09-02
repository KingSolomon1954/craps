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
        WINDOW *header,  *animation,
               *history, *houseBrief,
               *table,   *playerBrief,
               *message, *command;
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

    Craps::PlayerId userPlayerId_;
    Craps::TableId  tableId_;
    
    Windows w_;
    std::string lineBuffer_;
    Menus activeMenu_;
    TableView tableView_ = TableView::AllPlayers;
    size_t onePlayerIndex_ = 0;  // For cycling thru players
    std::vector<Craps::PlayerId> playerIds_;
    
    Windows createSubwindows();

    // Input handling
    void handleLineInput  (int ch);
    void handleMenuInput  (int ch);
    void handleAmountInput(int ch);
    void menuInputBetting (int ch);
    void menuInputStats   (int ch);
    void setMenuInputMode();
    void setLineInputMode();
    void cycleTableView();

    // Drawing table subwindow
    void drawTable();
    void drawHeader();
    void drawHistory();
    void drawTableAllPlayers();
    void drawTableOnePlayer();
    Craps::PlayerId getPlayerAt(size_t index);

    // Draw command subwindow
    void drawInputPrompt();

    void setupQuickBet();
    void setupAutoFill();
    void processAmountBuffer();
    void processLineBuffer();
};

// TODO: subscribe to players leaving/joining table
// TODO: update PlayerList upon notification

//----------------------------------------------------------------

} // namespace Cui

//----------------------------------------------------------------
