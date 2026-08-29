//----------------------------------------------------------------
//
// File: WindowPlayerArea.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/LayoutCrapsScreen.h>
#include <craps/CrapsTypes.h>

namespace Cui
{

class WindowPlayerArea
{
public:
    /// @name Lifecycle
    /// @{
    WindowPlayerArea();
   ~WindowPlayerArea() = default;
    /// @}

    /// @name Modifiers
    /// @{
    void draw() override;
    
    void nextPlayer();
    void prevPlayer();
    void allPlayers();
    /// @}

    /// @name EventHandlers
    void onPlayerJoined(const Craps::PlayerId& pid);
    void onPlayerLeft  (const Craps::PlayerId& pid);
    void onBetPlaced(const Craps::PlayerId& pid, Craps::BetId bid);
    void onBetFailed(const Craps::PlayerId& pid, const std::string& reason);
    /// @}

    /// @name Observers
    /// @{
    /// @}
    
private:
    struct Layout
    {
        using L = LayoutCrapsScreen;
        
        // Sizing and location based on LayoutCrapsScreen
        static constexpr int playerAreaTopRow   = L::playerAreaBorderTopRow   + 1;
        static constexpr int playerAreaBotRow   = L::playerAreaBorderBotRow   - 1;
        static constexpr int playerAreaLeftCol  = L::playerAreaBorderLeftCol  + 1;
        static constexpr int playerAreaRightCol = L::playerAreaBorderRightCol - 1;
        static constexpr int playerAreaHeight   = playerAreaBotRow   - playerAreaTopRow  + 1;
        static constexpr int playerAreaWidth    = playerAreaRightCol - playerAreaLeftCol + 1;

        static_assert(playerAreaTopRow   == 5);
        static_assert(playerAreaBotRow   == 23);
        static_assert(playerAreaLeftCol  == 1);
        static_assert(playerAreaRightCol == 68);
        static_assert(playerAreaHeight   == 19);
        static_assert(playerAreaWidth    == 68);
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

    enum class OneOrAll
    {
        OnePlayer,
        AllPlayers
    };

    Craps::PlayerId curPlayerId_;
    Craps::TableId  tableId_;
    std::vector<Craps::PlayerId> playerIds_;
    OneOrAll currentFocus_ = OneOrAll::AllPlayers;

    void initPlayers();
    void drawBorders();
    void drawExternalJunctions();
    void drawExternalJunctionsAllPlayers (WINDOW* pLendWin);
    void drawExternalJunctionsOnePlayer  (WINDOW* pLendWin);
    void eraseExternalJunctionsAllPlayers(WINDOW* pLendWin);
    void eraseExternalJunctionsOnePlayer (WINDOW* pLendWin);
    void drawInternalBorder();
    void drawInternalBordersAllPlayers()
    void drawInternalBordersOnePlayer()
    void drawStaticContent();
    void drawStaticContentAllPlayers();
    void drawStaticContentOnePlayer();
        
    void populate();
    void populateAllPlayers();
    void populateOnePlayer();
    void advancePlayer();
    Craps::PlayerId getNextPlayerId(const Craps::PlayerId& pid) const;
    Craps::PlayerId getPrevPlayerId(const Craps::PlayerId& pid) const;
};

} // namespace Cui

//----------------------------------------------------------------
