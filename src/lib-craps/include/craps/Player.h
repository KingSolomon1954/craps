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
#include <craps/PlayerStats.h>
#include <gen/ReturnCode.h>
#include <gen/MoneyUtils.h>
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
    Player(const PlayerId&     playerId, // Existing playerId,
           const PlayerConfig& config,   // name will come from file
           EventManager&       eventMgr);
   ~Player() = default;

    static Player* createPlayer(const std::string&  playerName,  // Creates
                                const PlayerConfig& config,      // a fresh
                                EventManager&       eventMgr);   // PlayerId
    static Player* fromString  (const std::string&  yaml,
                                const PlayerId&     playerId,
                                const PlayerConfig& config,
                                EventManager&       eventMgr);
    static Player* fromFile    (const PlayerId&     playerId,
                                const PlayerConfig& config,
                                EventManager&       eventMgr);
    /// @}

    /// @name Modifiers
    /// @{
    Gen::ReturnCode joinTable(CrapsTable* pTable, Gen::ErrorPass& ep);
    Gen::ReturnCode leaveTable(Gen::ErrorPass& ep);
    BetPtr makeBet(BetName betName,
                          Gen::Money contractAmount,
                          unsigned pivot,
                          Gen::ErrorPass& ep);
    Gen::ReturnCode setOddsAmount(BetPtr pBet,
                                  Gen::Money amount,
                                  Gen::ErrorPass& ep);
    Gen::ReturnCode removeBet(BetName betName,
                              unsigned pivot,
                              Gen::ErrorPass& ep);
    void processWin (const DecisionRecord& dr);
    void processLose(const DecisionRecord& dr);
    void processKeep(const DecisionRecord& dr);
    void resetStats();
    void prepareForShutdown();  // Shutdown, exiting program
    /// @}

    /// @name Observers
    /// @{
    const std::string& getName()                    const;
    const PlayerId&    getPlayerId()                const;
    Gen::Money         getAmountOnTable()           const;
    unsigned           getNumBetsOnTable()          const;
    Gen::Money         getBalance()                 const;
    const PlayerStats& getCurrentStats()            const;
    const PlayerStats& getAlltimeStats()            const;
    const BankStats&   getBankCurrentStats()        const;
    const BankStats&   getBankAlltimeStats()        const;
    bool               haveBet(const CrapsBet& bet) const;
    const SessionHistory::Sessions& getSessionHistory() const;

    /// @name File operations
    /// @{
    void saveFile() const;
    void loadFile();
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);
    /// @}

private:
    // order matters
    PlayerId          playerId_;
    PlayerConfig      config_;
    EventManager&     eventMgr_;
    Bank              wallet_;     // overriden by yaml

    // order doesn't matter
    std::string       playerName_; // set by yaml or creation
    std::string       shortDescription_;
    std::string       fullDescription_;
    std::list<BetPtr> bets_;
    CrapsTable*       pTable_;
    PlayerStats       currentStats_;
    PlayerStats       alltimeStats_;

    BetPtr makeShared(BetName betName,
                      Gen::Money contractAmount,
                      unsigned pivot,
                      Gen::ErrorPass& ep);
    BetPtr findBetById(BetId betId) const;
    bool removeBetByPtr(BetPtr& pBet);
    void setupSubscriptions();
    void onBettingClosed();
    void onBettingOpened();
    void onDiceThrowStart();
    void onDiceThrowEnd();
    void onAnnounceDiceNumber(const AnnounceDiceNumber& evt);
    void onPointEstablished  (const PointEstablished& evt);
    void onSevenOut();
    void onPassLineWinner();
    void onNewShooter(const NewShooter& evt);
    void setName(const std::string& playerName);

    // Validity checks with diagnostics
    void diagBadBetId(const std::string& funcName, BetId betId) const;
    std::string diagPrefix(size_t idx)                          const;
    bool fifNoTable(size_t idx, Gen::ErrorPass& ep)             const;
    bool fifMissingBet(BetPtr pBet, Gen::ErrorPass& ep)         const;
    bool fifInsufficientFunds(BetPtr pBet,  Gen::Money amount,
                              size_t idx, Gen::ErrorPass& ep)   const;
    bool fifBadAddBet (BetPtr pBet, Gen::ErrorPass& ep);
    bool fifBadSetOdds(BetPtr pBet, Gen::Money oddsAmount, Gen::ErrorPass& ep);

    // Default bank constants for player
    static constexpr unsigned InitialStartingBankBalance_ = 30000;
    static constexpr unsigned RefillThreshold_            = 15000;
    static constexpr unsigned RefillAmount_               = 20000;
};

/*-----------------------------------------------------------*//**

@class Player

@brief Craps player

Player Responsibilities:

@li Maintains player's money
@li Tracks player's bets
@li Know how to read/write its own data
@li Exports toYAML() and reads fromYAML()
@li Maintains one file per player
@li All player files located in the players directory

*/

} // namespace Craps

//----------------------------------------------------------------
