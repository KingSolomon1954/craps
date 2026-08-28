//----------------------------------------------------------------
//
// File: MenuOddsBet.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/MenuBase.h>
#include <craps/EnumBetName.h>
#include <craps/CrapsTypes.h>
#include <gen/MoneyUtils.h>
#include <gen/Timepoint.h>
#include <vector>

namespace Cui {

class MenuOddsBet : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuOddsBet() = default;
    /// @}

    /// @name Modifiers
    /// @{
    void draw()            override;
    void handleKey(int ch) override;
    /// @}

    /// @name Observers
    /// @{
    static MenuOddsBet& instance();
    /// @}
    
private:
    MenuOddsBet() = default;  // Private ctor, instead use instance()

    struct BetInfo
    {
        Craps::PlayerId playerId;
        Craps::BetId    betId;
        BetName         betName;
        unsigned        pivot;
        Gen::Money      contractAmount;
        Gen::Money      currentOddsAmount;
        Gen::Timepoint  whenCreated;
    };
    struct MenuEntry
    {
        char         hotKey;
        Craps::BetId betId;
        std::string  text;
    };
    
    using Bets        = std::vector<BetInfo>;
    using BetIdList   = std::vector<Craps::BetId>;
    using MenuEntries = std::vector<MenuEntry>;
    
    Craps::PlayerId playerId_;
    Bets            activeOddsBets_;
    MenuEntries     menuEntries_;
    int winBorderTopCol_ = 20;  // TODO from CUI layout
    int winBorderTopRow_ = 10;  // TODO from CUI layout
    
    // Gathering bets
    void gatherBets();
    void getPlayerId();
    void gatherBetsInfo();
    void getBetIdList(BetIdList& betIdList);
    void gatherBetsDetail(const BetIdList& betIdList);
    void sortBetsByCreated();

    // Creating menu entries
    void buildMenuEntries();
    void buildMenuBets();
    char indexToHotKey(size_t index);
    void addQuitEntry();
    std::string formatBet   (const BetInfo& bet);
    std::string formatAmount(const BetInfo& bet);
    std::string formatPivot (const BetInfo& bet);
    std::string formatHotKey(const char hotKey);

    // Window creation and drawing
    void windowResize();
    std::pair<int, int> calcSize() const;
    void drawBorders();
    void drawStaticContent();
    void populate();

    // Processing hotKeys
    void quit();
    void processSelection(const Craps::BetId& betId);
    BetInfo& findBet(const Craps::BetId& betId);
    void populateCarrier(const BetInfo& bet);
    void prepDialogAmount(const BetInfo& bet);
    void activateDialogAmount();
};

/*-----------------------------------------------------------*//**

@class MenuOddsBet

@brief Display choices for making odds bets

Responsibilities of MenuOddsBet:

@li Key bindings for the menu

@li Process input keys 

@li Takes action on input keys 

@li Renders the menu on screen

*/

} // namespace Cui

//----------------------------------------------------------------

