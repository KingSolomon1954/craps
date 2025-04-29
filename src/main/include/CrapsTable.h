//----------------------------------------------------------------
//
// File: CrapsTable.h
//
//----------------------------------------------------------------

#pragma once

#include <array>
#include <list>
#include <memory>
#include "gen/ErrorPass.h"
#include "gen/ReturnCode.h"
#include "gen/Uuid.h"
#include "Globals.h"
#include "CrapsBet.h"
#include "Dice.h"
#include "Player.h"

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
    // void resetTable();
    void addPlayer   (const Gen::Uuid& playerId);
    void removePlayer(const Gen::Uuid& playerId);

    Gen::ReturnCode addBet   (std::shared_ptr<CrapsBet> bet, Gen::ErrorPass& ep);
    Gen::ReturnCode removeBet(const std::shared_ptr<CrapsBet>& bet, Gen::ErrorPass& ep);
    
    void resolveRoll();           // Resolves bets for current roll
    
#if 0
    // Add a bet
    BetPtr newBet = std::make_shared<CrapsBet>(...);
    tableBets_[static_cast<size_t>(CrapsBetType::PassLine)].push_back(newBet);

    void addPlayer(const Player& player);
    void removePlayer(const std::string& playerName);
    void resetTable();

    Gen::ReturnCode addBet(const std::string& playerId, const CrapsBet& bet), Gen::ErrorPass& ep;
    Gen::ReturnCode removeBet(const CrapsBet& bet, Gen::ErrorPass& ep);
    void rollDice();              // Rolls two dice
    
    void startNewRound();         // Initiates come-out roll
    void advanceShooter();        // Move to next player/shooter

    bool isComeOutRoll() const;
    int  getPoint() const;        // Returns current point, or 0 if in come-out
    Dice getLastRoll() const;     // Get last dice roll
    const std::string& getCurrentShooter() const;
    std::vector<std::string> getPlayerList() const;
    std::vector<CrapsBet> getBetsForPlayer(const std::string& playerName) const;

#endif
    
    /// @}

    /// @name Observers
    /// @{
    /// @}
    
private:
    Dice dice;

    // Bets on the table are kept in a fixed sized array of lists, where
    // each array index equates to a bet type, and holds a list of bets
    // of that type. This allows easier traversals later that mimic real
    // life table actions where, for example, the house always first
    // collects losing bets in a certain order followed by payouts of
    // winning bets in a certain order.
    
    using BetPtr = std::shared_ptr<CrapsBet>;
    using BetList = std::list<BetPtr>;
    using BetTable = std::array<BetList, EnumBetName::enumerators.size()>;
//  using BetTable = std::array<BetList, static_cast<size_t>(BetName::Count)>;
    BetTable tableBets_;

    // Turn bet name enums into size_t to avoid casting each time.
    // Used when directly indexing into tableBets_;
    static inline constexpr size_t PlaceBetIndex = static_cast<size_t>(BetName::Place);
    

    static inline constexpr size_t MaxPlayers = 6;
    using Players = std::array<Gen::Uuid, MaxPlayers>;
    Players players_;
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
