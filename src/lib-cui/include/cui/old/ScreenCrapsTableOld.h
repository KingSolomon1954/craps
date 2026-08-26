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

class MenuBetting;  // fwd
    
class ScreenCrapsTable : public Screen
{
public:
   ~ScreenCrapsTable() override;
    static ScreenCrapsTable* instance();
    
    void draw()            override;
    void onAttach()        override;
    void handleKey(int ch) override;

    void onBetPlaced(const Craps::PlayerId& pid, Craps::BetId bid);
    void onBetFailed(const Craps::PlayerId& pid, const std::string& reason);
    void onPlayerJoined(const Craps::PlayerId& pid);
    void onPlayerLeft  (const Craps::PlayerId& pid);

private:
    struct Windows
    {
        WINDOW *header      = nullptr, *animation   = nullptr,
               *rollHistory = nullptr, *houseBrief  = nullptr,
               *playerArea  = nullptr, *playerBrief = nullptr,
               *message     = nullptr;
    };

    enum class PlayerArea
    {
        AllPlayers,
        OnePlayer
    };

    enum class Column
    {
        Left,
        Right
    };

    // Move this to ScreenBase or ViewSurface or ?
    struct Layout
    {
        // Minimum supported terminal size 41x100
        static constexpr int minTerminalRows = 41;
        static constexpr int minTerminalCols = 100;
        
        static constexpr int screenBorderRowTop = 0;
        static constexpr int screenBorderRowBot = 40;
        static constexpr int screenHeight = minTerminalRows;
        
        static constexpr int screenBorderColLeft = 0;
        static constexpr int screenBorderColRight = 99;
        static constexpr int screenWidth = minTerminalCols;
    };

    struct LayoutCrapsScreen
    {
        // Header Window
        static constexpr int headerBorderTopRow   = 0;
        static constexpr int headerBorderBotRow   = 2;
        static constexpr int headerBorderLeftCol  = 0;
        static constexpr int headerBorderRightCol = 69;
        static constexpr int headerWinTopRow      = 1;
        static constexpr int headerWinBotRow      = 1;
        static constexpr int headerWinLeftCol     = 1;
        static constexpr int headerWinRightCol    = 68;
        static constexpr int headerWinHeight      = 1;
        static constexpr int headerWinWidth       = 67;

        // Dice Roll History Window
        static constexpr int rollHistBorderTopRow   = 2;
        static constexpr int rollHistBorderBotRow   = 4;
        static constexpr int rollHistBorderLeftCol  = 0;
        static constexpr int rollHistBorderRightCol = 69;
        static constexpr int rollHistWinTopRow      = 3;
        static constexpr int rollHistWinBotRow      = 3;
        static constexpr int rollHistWinLeftCol     = 1;
        static constexpr int rollHistWinRightCol    = 68;
        static constexpr int rollHistWinHeight      = 1;
        static constexpr int rollHistWinWidth       = 67;
        
        // Player Table Area Window
        static constexpr int playerAreaBorderTopRow   = 4;
        static constexpr int playerAreaBorderBotRow   = 24;
        static constexpr int playerAreaBorderLeftCol  = 0;
        static constexpr int playerAreaBorderRightCol = 69;
        static constexpr int playerAreaWinTopRow      = 5;
        static constexpr int playerAreaWinBotRow      = 23;
        static constexpr int playerAreaWinLeftCol     = 1;
        static constexpr int playerAreaWinRightCol    = 68;
        static constexpr int playerAreaWinHeight      = 19;
        static constexpr int playerAreaWinWidth       = 67;
        
        // Message Area Window
        static constexpr int messageBorderTopRow   = 24;
        static constexpr int messageBorderBotRow   = 38;
        static constexpr int messageBorderLeftCol  = 0;
        static constexpr int messageBorderRightCol = 69;
        static constexpr int messageWinTopRow      = 25;
        static constexpr int messageWinBotRow      = 37;
        static constexpr int messageWinLeftCol     = 1;
        static constexpr int messageWinRightCol    = 68;
        static constexpr int messageWinHeight      = 13;
        static constexpr int messageWinWidth       = 67;
        
        // Dice Animation Window
        static constexpr int animationBorderTopRow   = 0;
        static constexpr int animationBorderBotRow   = 24;
        static constexpr int animationBorderLeftCol  = 69;
        static constexpr int animationBorderRightCol = 99;
        static constexpr int animationWinTopRow      = 1;
        static constexpr int animationWinBotRow      = 23;
        static constexpr int animationWinLeftCol     = 70;
        static constexpr int animationWinRightCol    = 98;
        static constexpr int animationWinHeight      = 23;
        static constexpr int animationWinWidth       = 24;
        
        // House Brief Info Window
        static constexpr int houseBriefBorderTopRow   = 24;
        static constexpr int houseBriefBorderBotRow   = 30;
        static constexpr int houseBriefBorderLeftCol  = 69;
        static constexpr int houseBriefBorderRightCol = 99;
        static constexpr int houseBriefWinTopRow      = 25;
        static constexpr int houseBriefWinBotRow      = 29;
        static constexpr int houseBriefWinLeftCol     = 70;
        static constexpr int houseBriefWinRightCol    = 98;
        static constexpr int houseBriefWinHeight      = 5;
        static constexpr int houseBriefWinWidth       = 24;
        
        // Player Brief Info Window
        static constexpr int playerBriefBorderTopRow   = 30;
        static constexpr int playerBriefBorderBotRow   = 38;
        static constexpr int playerBriefBorderLeftCol  = 69;
        static constexpr int playerBriefBorderRightCol = 99;
        static constexpr int playerBriefWinTopRow      = 31;
        static constexpr int playerBriefWinBotRow      = 37;
        static constexpr int playerBriefWinLeftCol     = 70;
        static constexpr int playerBriefWinRightCol    = 98;
        static constexpr int playerBriefWinHeight      = 7;
        static constexpr int playerBriefWinWidth       = 24;
        
        // NavBar Window TODO make this a separate class
        static constexpr int navBarBorderTopRow   = 38;
        static constexpr int navBarBorderBotRow   = 40;
        static constexpr int navBarBorderLeftCol  = 0;
        static constexpr int navBarBorderRightCol = 99;
        static constexpr int navBarWinTopRow      = 39;
        static constexpr int navBarWinBotRow      = 39;
        static constexpr int navBarWinLeftCol     = 1;
        static constexpr int navBarWinRightCol    = 98;
        static constexpr int navBarWinHeight      = 1;
        static constexpr int navBarWinWidth       = 98;
        
        // Dice Drawing
        // TODO move this to animation window rendering 
        static constexpr int diceHeight = 5;
        static constexpr int diceWidth = 7;
    };

    struct LayoutAllPlayers
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
    Craps::PlayerId  userPlayerId_;
    Craps::TableId   tableId_;
    Windows          w_;
    std::string      lineBuffer_;
    PlayerArea       playerArea_ = PlayerArea::AllPlayers;
    size_t           onePlayerIndex_ = 0;  // For cycling thru players
    MenuBetting*     pMenuBetting_ = nullptr;
    std::vector<Craps::PlayerId> playerIds_;
    
    ScreenCrapsTable();  // ctor is private
    
    void cyclePlayerArea();

    // Drawing screen borders
    void drawCrapsScreen();
    void drawBorders();
    void drawJunctionsAllPlayers();
    void drawJunctionsOnePlayer();
    void eraseJunctionsAllPlayers();
    void eraseJunctionsOnePlayer();
    
    // Populating table windows
    void populateHeader();
    void populateRollHistory();
    void populatePlayerArea();
    void populateMessages();
    void populateAnimation();
    void populateHouseBrief();
    void populatePlayerBrief();
    void populateAllPlayers();
    void populateOnePlayer();
    
    void createContentWindows();
    Craps::PlayerId getPlayerAt(size_t index);
};

/*-----------------------------------------------------------*//**

@class ScreenCrapsTable

@brief Craps Table Screen implementation

@li owns size of each window
@li relationship between windows
@li screen-level borders
@li screen-level junctions
@li overall composition

Draws something like this.

                    ScreenCrapsTable
 ┌──────────────────────────────────────┬──────────────┐
 │              Header                  │    Dice      │
 ├──────────────────────────────────────┤  Animation   │
 │           Roll History               │              │
 ├──────────────────────────────────────┤              │
 │                                      │              │
 │          Player Area                 │              │
 │                                      │              │
 ├──────────────────────────────────────┼──────────────┤
 │             Messages                 │ House Brief  │
 │                                      ├──────────────┤
 │                                      │ Player Brief │
 ├──────────────────────────────────────┴──────────────┤
 │                        NavBar                       │
 └─────────────────────────────────────────────────────┘

*/

// TODO: subscribe to players leaving/joining table
// TODO: update PlayerList upon notification

//----------------------------------------------------------------

} // namespace Cui

//----------------------------------------------------------------
