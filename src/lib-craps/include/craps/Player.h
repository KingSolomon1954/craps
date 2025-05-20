//----------------------------------------------------------------
//
// File: Player.h
//
//----------------------------------------------------------------

#pragma once

#include <string>
#include <memory>
#include <list>
#include <nlohmann/json.hpp>
#include <controller/Globals.h>
#include "craps/Bank.h"
#include "craps/Events.h"
#include "craps/EnumBetName.h"
#include "gen/ErrorPass.h"
#include "gen/ReturnCode.h"
#include "gen/Uuid.h"

using json = nlohmann::json;

namespace Craps {

class CrapsBetIntfc;    // fwd
class DecisionRecord;   // fwd
class ErrorPass;        // fwd

class Player
{
public:
    /// @name Lifecycle
    /// @{
    Player();
    Player(const std::string& name,
           unsigned startingBalance);
    Player(const Gen::Uuid&,
           const std::string& name,
           unsigned startingBalance);

    bool saveToFile(const std::string& path) const;
    bool loadFromFile(const std::string& path);

//    static Player deserialize(const std::string& line);
//    std::string serialize() const;
    /// @}

#if 0
class CrapsInterface
{
    // Table related (TableManager)
    using TableId = std::string;
    using TableList = std::vector<TableId>;
    static TableList getTableList() const;
    static TablePickerView getTablePickerView(TableId tableId);
    static TableGameView getTableGameView(TableId tableId);
    static TableStatsView getTableStatsView(TableId tableId);

    // Player related (PlayerManager)
    using PlayerId = std::string;
    using PlayerList = std::vector<PlayerId> ;
    static PlayerList getPlayerList() const;
    static PlayerPickerView getPlayerPickerView(PlayerId playerId);
    static PlayerGameView getPlayerGameView(PlayerId playerId);
    static PlayerStatsView getPlayerStatsView(TableId tableId);

    static Gen::ReturnCode openTable(TableId tableId, PlayerList p);
    static Gen::ReturnCode closeTable(TableId tableId);

    using BetId = unsigned;
    using BetList = std::list<BetId>;
    using Money = unsigned;

    static ReturnCode join(TableId tableId, PlayerId playerId, Gen::ErrorPass& ep);
    static ReturnCode leave(TableId tableId, PlayerId playerId, Gen::ErrorPass& ep);
    
    static BetId placeBet(TableId tableId,
                          PlayerId playerId,
                          BetNameStr betName,
                          Gbl::Money contractAmount,
                          Gen::ErrorPass& ep);
    static ReturnCode removeBet(TableId tableId, BetId bet, Gen::ErrorPass& ep);
    static ReturnCode setOdds(TableId tableId, BetId bet, Gbl::Money oddsAmount, Gen::ErrorPass& ep);
    static ReturnCode changeBetAmount(TableId tableId, BetId bet, int delta, Gen::ErrorPass& ep);
    static void rollDice(TalbeId tableId);
    
};    
#endif
    
    
    /// @name Modifiers
    /// @{
    Gen::ReturnCode joinTable(Gen::ErrorPass& ep);
    Gen::ReturnCode makeBet(BetName betName,
                            Gbl::Money contractAmount,
                            unsigned pivot,
                            Gen::ErrorPass& ep);
    Gen::ReturnCode removeBet(BetName betName,
                              unsigned pivot,
                              Gen::ErrorPass& ep);
    void processWin (const DecisionRecord& dr);
    void processLose(const DecisionRecord& dr);
    void processKeep(const DecisionRecord& dr);
    void fromJson(const json& j);
    /// @}

    /// @name Observers
    /// @{
    const Gen::Uuid& getUuid()   const;
    const std::string& getName() const;
    Gbl::Money getAmountOnTable()     const;
    unsigned getNumBetsOnTable() const;
    Gbl::Money getBalance()           const;
    json toJson()                const;
    /// @}

private:
    Gen::Uuid uuid_;
    std::string name_;
    Bank wallet_;
    using BetIntfcPtr = std::shared_ptr<class CrapsBetIntfc>;
    std::list<BetIntfcPtr> bets_;

    bool removeBetByPtr(BetIntfcPtr& pBet);
    BetIntfcPtr findBetById(unsigned betId) const;
    void diagBadBetId(const std::string& funcName, unsigned betId) const;
    void setupSubscriptions();
    void onBettingClosed();
    void onBettingOpened();
    void onDiceThrowStart();
    void onDiceThrowEnd();
    void onAnnounceDiceNumber(const AnnounceDiceNumber& evt);
    void onPointEstablished(const PointEstablished& evt);
    void onSevenOut();
    void onPassLineWinner();
    void onNewShooter(const NewShooter& evt);
};

/*-----------------------------------------------------------*//**

@class Player

@brief Craps player

Player Responsibilities:

@li maintains one file per player

@li all player files in the players directory

@li Know how to read/write its own data

@li Export a to_json() and from_json() method

*/

} // namespace Craps

//----------------------------------------------------------------
