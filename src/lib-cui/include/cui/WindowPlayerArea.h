//----------------------------------------------------------------
//
// File: WindowPlayerArea.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/LayoutCrapsScreen.h>

namespace Cui
{

struct WINDOW; // fwd
    
class PlayerArea
{
public:
    /// @name Lifecycle
    /// @{
    PlayerArea();
   ~PlayerArea();
    /// @}

    /// @name Modifiers
    /// @{
    void draw();
    void update();
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

        // static constexpr int playerAreaTopRow   = 5;
        // static constexpr int playerAreaBotRow   = 23;
        // static constexpr int playerAreaLeftCol  = 1;
        // static constexpr int playerAreaRightCol = 68;
        // static constexpr int playerAreaHeight   = 19;
        // static constexpr int playerAreaWidth    = 67;
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

    enum class PlayerArea
    {
        AllPlayers,
        OnePlayer
    };

    WINDOW* pWindow_ = nullptr;  // The ncurses window
    PlayerArea playerArea_ = PlayerArea::AllPlayers;
    
    void drawBorders();
    void drawExteralJunctions();
    void drawExternalJunctionsAllPlayers();
    void drawExternalJunctionsOnePlayer();
    void eraseExternalJunctionsAllPlayers();
    void eraseExternalJunctionsOnePlayer();
    void drawInternalBorder();
    void drawInternalBordersAllPlayers()
    void drawInternalBordersOnePlayer()
    void drawStaticContent();
    void drawStaticContentAllPlayers();
    void drawStaticContentOnePlayer();
        
    void populate();
    void populateAllPlayers();
    void populateOnePlayer();
    
    void cyclePlayerArea();  // TODO change into next and previous
    // size_t           onePlayerIndex_ = 0;  // For cycling thru players
    // auto playerId = playerIds_[onePlayerIndex_];

};

} // namespace Cui

//----------------------------------------------------------------
