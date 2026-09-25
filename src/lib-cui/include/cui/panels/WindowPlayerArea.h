//----------------------------------------------------------------
//
// File: WindowPlayerArea.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/PanelBase.h>
#include <cui/layouts/LayoutCrapsScreen.h>
#include <cui/panels/PlayerAreaAllPlayers.h>
#include <cui/panels/PlayerAreaOnePlayer.h>
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

    Craps::PlayerId              curPlayerId_;
    std::vector<Craps::PlayerId> playerIds_;
    PlayerAreaAllPlayers         allPlayersView_;
    PlayerAreaOnePlayer          onePlayerView_;
    OneOrAll                     currentFocus_ = OneOrAll::OnePlayer;

private:
    WindowPlayerArea();
    void initPlayers();
    void createWindow();
    void drawExternalJunctions();
    void drawInternalBorders();
    void drawStaticContent();
    void drawStaticContentAllPlayers();
    void drawStaticContentOnePlayer();
        
    void populate();
    void populateAllPlayers();
    void populateOnePlayer();
    void advancePlayer(bool next);
    Craps::PlayerId getNextPlayerId(const Craps::PlayerId& pid) const;
    Craps::PlayerId getPrevPlayerId(const Craps::PlayerId& pid) const;
};

} // namespace Cui

//----------------------------------------------------------------
