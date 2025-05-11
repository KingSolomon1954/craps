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
    Gen::ReturnCode addPlayer   (const Gen::Uuid& playerId, Gen::ErrorPass& ep);
    Gen::ReturnCode removePlayer(const Gen::Uuid& playerId, Gen::ErrorPass& ep);
    Gen::ReturnCode updatePlayerId(const Gen::Uuid& oldId,
                                   const Gen::Uuid& newId,
                                   Gen::ErrorPass& ep);

    using BetIntfcPtr = std::shared_ptr<CrapsBetIntfc>;
    BetIntfcPtr addBet(const Gen::Uuid& playerId,
                       BetName betName,
                       Money contractAmount,
                       unsigned pivot,
                       Gen::ErrorPass& ep);
    Gen::ReturnCode changeBetAmount (BetIntfcPtr pBet, int delta, Gen::ErrorPass& ep);
    Gen::ReturnCode removeBet       (BetIntfcPtr pBet, Gen::ErrorPass& ep);
    Gen::ReturnCode setOdds         (BetIntfcPtr bet, Money amount, Gen::ErrorPass& ep);
    
    void rollDice();
    void testSetState(unsigned point, unsigned d1, unsigned d2);
    /// @}

    /// @name Observers
    /// @{
    bool havePlayer(const Gen::Uuid& playerId) const;
    unsigned getNumPlayers()                   const;
    std::vector<Gen::Uuid> getPlayers()        const;
    unsigned getPoint()                        const;
    Gen::Uuid getIdShooter()                   const;
    Dice getLastRoll()                         const;
    bool isComeOutRoll()                       const;
    bool isBettingOpen()                       const;
    Money getAmountOnTable()                   const; 
    unsigned getNumBetsOnTable()               const;
    bool haveBet(const BetIntfcPtr bet)        const;
    bool haveBet(const Gen::Uuid& playerId, BetName betName, unsigned pivot) const;
    /// @}
    
#if 0
    void resetTable();
    void startNewRound();         // Initiates come-out roll
    void advanceShooter();        // Move to next player/shooter
    std::vector<CrapsBet> getBetsForPlayer(const std::string& playerName) const;

#endif
    
private:
    Dice dice_;
    unsigned point_ = 0;
    Gen::Uuid shooterId_;
    bool bettingOpen_ = true;

    // Players must join table in order to play.  We only hold the
    // player's UUID here in a std::list container and rely on the
    // PlayerManager to interface with players.
    // 
    static inline constexpr size_t MaxPlayers = 6;
    using PlayerList = std::list<Gen::Uuid>;
    PlayerList players_;
    Gen::ReturnCode removeUuid(const Gen::Uuid& id, Gen::ErrorPass& ep);
    Gen::ReturnCode updateUuid(const Gen::Uuid& oldId,
                               const Gen::Uuid& newId,
                               Gen::ErrorPass& ep);
    
    // Bets on the table are kept in a fixed sized array of lists, where
    // each array index equates to a bet type, and holds a list of bets
    // of that type. This allows easier traversals later that mimic real
    // life table actions where, for example, the house always first
    // collects losing bets in a certain order followed by payouts of
    // winning bets in a certain order.
    //
    using BetList = std::list<BetIntfcPtr>;
    using BetTable = std::array<BetList, EnumBetName::enumerators.size()>;
    BetTable tableBets_;

    // Turn bet name enums into size_t to avoid casting each time.
    // Used when directly indexing into tableBets_;
    static inline constexpr size_t PlaceBetIndex = static_cast<size_t>(BetName::Place);

    bool betAllowed(const Gen::Uuid& playerId, BetName betName,
                    unsigned& pivot, Gen::ErrorPass& ep) const;
    void resolveBets();
    void advanceState();
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
