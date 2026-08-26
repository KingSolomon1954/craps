//----------------------------------------------------------------
//
// File: MenuOdds.h
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

class ConsoleView;  // fwd
    
class MenuOdds : public MenuBase
{
public:
    struct Results
    {
        bool            canceled;
        Craps::PlayerId playerId;
        BetName         betName;
        Craps::BetId    betId;
        size_t          pivot;
        Gen::Money      contractAmount;
        Gen::Money      currentOddsAmount;
    };
    
    /// @name Lifecycle
    /// @{
    static MenuOdds& instance();
   ~MenuOdds();
    /// @}

    /// @name Modifiers
    /// @{
    void clearState();
    /// @}

    /// @name Observers
    /// @{
    Results getResults() const;
    /// @}
    
protected:
    void drawMenu()            override;
    void handleMenuKey(int ch) override;

private:
    struct BetEntry
    {
        Craps::PlayerId playerId;
        Craps::BetId    betId;
        BetName         betName;
        unsigned        pivot;
        Gen::Money      contractAmount;
        Gen::Money      currentOddsAmount;
        Gen::Timepoint  whenCreated;
    };
    using Bets = std::vector<BetEntry>;

    Craps::PlayerId playerId_;
    BetName         betName_;
    Craps::BetId    betId_;
    size_t          pivot_;
    Gen::Money      contractAmount_;
    Gen::Money      currentOddsAmount_;
    Bets            bets_;
    
    MenuOdds();  // Private ctor
    
    void buildOddsBets();
    void doSelection(BetEntry& bet);
    void sortBetsByCreated();
    int  betIndexFromKey(int ch, size_t betCount);
    char betKeyFromIndex(size_t idx);
    void buildOddsBetEntries();
    void buildMenuEntries(std::vector<std::string>& entries);
    void renderMenuEntries(const std::vector<std::string>& entries, int startRow);
    int  getLongestEntry(const std::vector<std::string>& entries,
                         const std::string& title,
                         const std::string&  prompt) const;
    void populateBets(std::vector<Craps::BetId>& betIds);
};

/*-----------------------------------------------------------*//**

@class MenuOdds

@brief Display choices for making odds bets

Responsibilities of MenuOdds:

@li Key bindings for the menu

@li Process input keys 

@li Takes action on input keys 

@li Renders the menu on screen

*/

} // namespace Cui

//----------------------------------------------------------------

