//----------------------------------------------------------------
//
// File: WindowPlayerArea.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/PanelBase.h>
#include <cui/layouts/LayoutCrapsScreen.h>
#include <craps/CrapsTypes.h>
#include <vector>

namespace Cui
{

    class WindowPlayerArea : PanelBase
{
public:
    /// @name Lifecycle
    /// @{
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
    static WindowPlayerArea& instance();
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

    enum class OneOrAll
    {
        OnePlayer,
        AllPlayers
    };

    Craps::PlayerId curPlayerId_;
    Craps::TableId  tableId_;
    std::vector<Craps::PlayerId> playerIds_;
    OneOrAll currentFocus_ = OneOrAll::AllPlayers;

    WindowPlayerArea();    // Private ctor
    void initPlayers();
    void createWindow();
    void registerWindow();
    void drawExternalJunctions();
    void drawInternalBorders();
    void drawInternalBordersAllPlayers();
    void drawInternalBordersOnePlayer();
    void drawStaticContent();
    void drawStaticContentAllPlayers();
    void drawStaticContentOnePlayer();
        
    void populate();
    void populateAllPlayers();
    void populateOnePlayer();
    void advancePlayer(bool next);
    Craps::PlayerId getNextPlayerId(const Craps::PlayerId& pid) const;
    Craps::PlayerId getPrevPlayerId(const Craps::PlayerId& pid) const;

// TODO: subscribe to players leaving/joining table
// TODO: update PlayerList upon notification

};

} // namespace Cui

//----------------------------------------------------------------
