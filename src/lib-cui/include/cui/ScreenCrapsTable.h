//----------------------------------------------------------------
//
// File: ScreenCrapsTable.h
//
//----------------------------------------------------------------

#pragma once

#include <ncurses.h>
#include <cui/ScreenBase.h>
#include <cui/ConsoleLayout.h>
#include <craps/CrapsTypes.h>

namespace Cui {

class MenuBetting;  // fwd
    
class ScreenCrapsTable : public ScreenBase
{
public:
    /// @name Lifecycle
    /// @{
   ~ScreenCrapsTable() override;
    static ScreenCrapsTable* instance();
    /// @}
    
    /// @name Modifiers
    /// @{
    void draw()            override;  // from ViewSurface
    void onAttach()        override;  // from ViewSurface
    void handleKey(int ch) override;  // from ViewSurface
    /// @}

    void onBetPlaced(const Craps::PlayerId& pid, Craps::BetId bid);
    void onBetFailed(const Craps::PlayerId& pid, const std::string& reason);
    void onPlayerJoined(const Craps::PlayerId& pid);
    void onPlayerLeft  (const Craps::PlayerId& pid);

    /// @name Observers
    /// @{
    /// @}

private:

    // Order doesn't matter    
    Craps::PlayerId  userPlayerId_;
    Craps::TableId   tableId_;
    std::string      lineBuffer_;
    MenuBetting*     pMenuBetting_ = nullptr;
    std::vector<Craps::PlayerId> playerIds_;

    // Window Classes
    Header*      pHeader_;
    RollHistory* pRollHistory_;
    WindowPlayerArea*  pPlayerArea_;
    Messages*    pMessages_;
    Animation*   pAnimation_;
    HouseBrief*  pHouseBrief_;
    PlayerBrief* pPlayerBrief_;
    
    ScreenCrapsTable();  // ctor is private
    
    void drawCrapsScreen();
    void transfer();  // Move window contents to ncurses backing store
    
    // Populating table windows
    void populateHeader();
    void populateRollHistory();
    void populateMessages();
    void populateAnimation();
    void populateHouseBrief();
    void populatePlayerBrief();
    void populateAllPlayers();
    void populateOnePlayer();
    
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

 
*/

// TODO: subscribe to players leaving/joining table
// TODO: update PlayerList upon notification

//----------------------------------------------------------------

} // namespace Cui

//----------------------------------------------------------------
