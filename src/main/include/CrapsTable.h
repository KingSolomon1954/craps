//----------------------------------------------------------------
//
// File: CrapsTable.h
//
//----------------------------------------------------------------

#pragma once

#include "Globals.h"
#include "CrapsBet.h"
#include "Dice.h"

namespace App {

class CrapsTable
{
public:
    /// @name Lifecycle
    /// @{
    CrapsTable();
   ~CrapsTable() = default;
    /// @}

    /// @name Modifiers
    /// @{
#if 0

    class TableBet
    {
        CrapsPlayer* pPlayer;
        CrapsBet* pBet;
    };

    // Bets on the table are kept in a fixed sized array with each array
    // index representing a list of bets of that type.  This allows
    // easier traversals that mimic real life table actions where, for
    // example, the house always collects losing bets in a certain order
    // followed by payouts in a certain order.
    std::unordered_map<BetName, std::list<shared_ptr<CrapsBet>>>
    
    void addPlayer(const Player& player);
    void removePlayer(const std::string& playerName);
    void resetTable();

    Gen::ReturnCode placeBet(const std::string& playerId, const CrapsBet& bet), Gen::ErrorPass& ep;
    Gen::ReturnCode removeBet(const CrapsBet& bet, Gen::ErrorPass& ep);
    
    void startNewRound();         // Initiates come-out roll
    void rollDice();              // Rolls two dice
    void resolveRoll();           // Resolves bets for current roll
    void advanceShooter();        // Move to next player/shooter

    bool isComeOutRoll() const;
    int  getPoint() const;        // Returns current point, or 0 if in come-out
    Dice getLastRoll() const;     // Get last dice roll
    const std::string& getCurrentShooter() const;
    std::vector<std::string> getPlayerList() const;
    std::vector<CrapsBet> getBetsForPlayer(const std::string& playerName) const;

    std::vector<std::shared_ptr<CrapsBet>> tableBets;

    // Add new bet
    tableBets.push_back(std::make_shared<CrapsBet>(player, PassLine, 50.0));
    tableBets.push_back(std::make_shared<CrapsBet>(player, PassLine, 50.0));

#endif
    
    /// @}

    /// @name Observers
    /// @{
    /// @}
    
private:
    Dice dice;
};

/*-----------------------------------------------------------*//**

@class CrapsTable

@brief Craps Table.

Operates a Craps Table.

Responsibilities:

* interacts with players, bets, dice rolls, and house bank
* manages players and their bets
* accepts/rejects bets from players
* handles dice rolls and game phases
* processes dice rolls and bets
* gives win amounts to player
* processes payouts and bet resolutions
* removes wining/losing bets from table
* gives losing amounts to table bank
* refills table money from the casino bank
* controls who the shooter is
* passes dice to the shooter
* maintain stats on table play
* keeps track of the table state (point, current shooter, etc.)

*/

} // namespace App

//----------------------------------------------------------------
