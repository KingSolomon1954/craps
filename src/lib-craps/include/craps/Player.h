//----------------------------------------------------------------
//
// File: Player.h
//
//----------------------------------------------------------------

#pragma once

#include <string>
#include <memory>
#include <list>
#include <craps/Events.h>
#include <craps/Bank.h>
#include <craps/CrapsBet.h>
#include <craps/CrapsTypes.h>
#include <craps/EnumBetName.h>
#include <craps/PlayerConfig.h>
#include <gen/ReturnCode.h>
#include <gen/MoneyUtil.h>
#include <yaml-cpp/yaml.h>

namespace Gen {
    class ErrorPass;    // fwd
}

namespace Craps {

class DecisionRecord;   // fwd
class EventManager;     // fwd
class CrapsTable;       // fwd

class Player
{
public:
    /// @name Lifecycle
    /// @{
    Player(const Craps::PlayerId& playerId, // Existing playerId, name comes from file
           const PlayerConfig&    config,
           EventManager&          eventMgr);

    Player createPlayer(const std::string&  playerName,  // Will create a fresh PlayerId)
                        const PlayerConfig& config,
                        EventManager&       eventMgr);
    
    static Player* fromConfig(const Craps::PlayerId& playerId,
                              const PlayerConfig&    config,
                              EventManager&          eventMgr);
    static Player* fromFile  (const Craps::PlayerId& playerId,
                              const PlayerConfig&    config,
                              EventManager&          eventMgr);
    /// @}

    /// @name Modifiers
    /// @{
    Gen::ReturnCode joinTable(CrapsTable* pTable, Gen::ErrorPass& ep);
    CrapsBet::BetPtr makeBet(BetName betName,
                             Gen::Money contractAmount,
                             unsigned pivot,
                             Gen::ErrorPass& ep);
    Gen::ReturnCode setOddsAmount(CrapsBet::BetPtr pBet,
                                  Gen::Money amount,
                                  Gen::ErrorPass& ep);
    Gen::ReturnCode removeBet(BetName betName,
                              unsigned pivot,
                              Gen::ErrorPass& ep);
    void processWin (const DecisionRecord& dr);
    void processLose(const DecisionRecord& dr);
    void processKeep(const DecisionRecord& dr);
    /// @}

    /// @name Observers
    /// @{
    const std::string&     getName()           const;
    const Craps::PlayerId& getPlayerId()       const;
    Gen::Money             getAmountOnTable()  const;
    unsigned               getNumBetsOnTable() const;
    Gen::Money             getBalance()        const;

    // File operations
    void saveFile() const;
    void loadFile();
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    /// @}

private:
    // order matters
    Craps::PlayerId             playerId_;
    PlayerConfig                config_;
    EventManager&               eventMgr_;
    Bank                        wallet_;     // overriden by yaml

    // order doesn't matter
    std::string                 playerName_; // set by yaml or creation
    std::list<CrapsBet::BetPtr> bets_;
    CrapsTable*                 pTable_;

    bool removeBetByPtr(CrapsBet::BetPtr& pBet);
    CrapsBet::BetPtr findBetById(unsigned betId) const;
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
    void setName(const std::string& playerName);

    // Default bank constants for player
    static constexpr unsigned InitialStartingBankBalance_ = 30000;
    static constexpr unsigned RefillThreshold_            = 15000;
    static constexpr unsigned RefillAmount_               = 20000;
};

/*-----------------------------------------------------------*//**

@class Player

@brief Craps player

Player Responsibilities:

@li maintains one file per player

@li all player files in the players directory

@li Know how to read/write its own data

@li Exports toYAML() and reads fromYAML()

*/

} // namespace Craps

//----------------------------------------------------------------
