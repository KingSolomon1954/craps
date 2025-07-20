//----------------------------------------------------------------
//
// File: CrapsTable.h
//
//----------------------------------------------------------------

#pragma once

#include <array>
#include <deque>
#include <list>
#include <memory>
#include <gen/ErrorPass.h>
#include <gen/MoneyUtil.h>
#include <gen/ReturnCode.h>
#include <gen/Timepoint.h>
#include <gen/Uuid.h>
#include <craps/Bank.h>
#include <craps/CrapsBet.h>
#include <craps/DecisionRecord.h>
#include <craps/Dice.h>
#include <craps/Player.h>
#include <craps/TableStats.h>

namespace Craps {

class CrapsTable
{
public:
    using TableId = std::string;
    using BetPtr = std::shared_ptr<class CrapsBet>;
    
    /// @name Lifecycle
    /// @{
    CrapsTable(const TableId& tableId);
   ~CrapsTable() = default;
    static CrapsTable* fromConfig(const TableId& tableId);
    static CrapsTable* fromFile  (const TableId& tableId);
    /// @}

    /// @name Modifiers
    /// @{
    Gen::ReturnCode addPlayer   (const Gen::Uuid& playerId, Gen::ErrorPass& ep);
    Gen::ReturnCode removePlayer(const Gen::Uuid& playerId, Gen::ErrorPass& ep);
    Gen::ReturnCode updatePlayerId(const Gen::Uuid& oldId,
                                   const Gen::Uuid& newId,
                                   Gen::ErrorPass& ep);
    Gen::ReturnCode addBet   (BetPtr pBet, Gen::ErrorPass& ep);
    Gen::ReturnCode removeBet(BetPtr pBet, Gen::ErrorPass& ep);

#if 0    
    BetIntfcPtr addBet(const Gen::Uuid& playerId,
                       BetName betName,
                       Gen::Money contractAmount,
                       unsigned pivot,
                       Gen::ErrorPass& ep);
    
    Gen::ReturnCode changeBetAmount (BetIntfcPtr pBet, int delta, Gen::ErrorPass& ep);
    Gen::ReturnCode removeBet       (BetIntfcPtr pBet, Gen::ErrorPass& ep);
    Gen::ReturnCode setOdds         (BetIntfcPtr bet, Gen::Money amount, Gen::ErrorPass& ep);
#endif
    
    void rollDice();
    void testRollDice(unsigned d1, unsigned d2);
    void testSetState(unsigned point, unsigned d1, unsigned d2);
    void resetStats();
    void close();               // Shutdown table, soon to switch out.
    void prepareForShutdown();  // Shutdown table, soon to destruct.
    /// @}

    /// @name Observers
    /// @{
    unsigned                getNumPlayers()     const;
    unsigned                getPoint()          const;
    Dice                    getCurRoll()        const;
    Gen::Money              getAmountOnTable()  const;
    unsigned                getNumBetsOnTable() const;
    std::vector<Gen::Uuid>  getPlayers()        const;
    Gen::Uuid               getShooterId()      const;
    const std::deque<Dice>& getRecentRolls()    const;
    const TableStats&       getCurrentStats()   const;
    const TableStats&       getAlltimeStats()   const;
    unsigned                getMinLineBet()     const;
    unsigned                getMaxLineBet()     const;
    unsigned                getMaxOdds()        const;
    bool                    isComeOutRoll()     const;
    bool                    isBettingOpen()     const;
    bool                    havePlayer(const Gen::Uuid& playerId) const;
    bool                    haveBet(const BetPtr bet)             const;
    bool                    haveBet(const Gen::Uuid& playerId,
                                    BetName betName,
                                    unsigned pivot) const;
    bool                    withinTableLimits(BetName betName,
                                              Gen::Money contractAmount,
                                              Gen::ErrorPass& ep) const;
    const SessionHistory::Sessions& getSessionHistory() const;
    /// @}

#if 0
    void resetTable();
    void startNewRound();         // Initiates come-out roll
    std::vector<CrapsBet> getBetsForPlayer(const std::string& playerName) const;

#endif

private:
    TableId tableId_;
    std::string tableName_;
    std::string shortDescription_;
    std::string fullDescription_;
    unsigned minLineBet_ = 5;           // overriden by yaml
    unsigned maxLineBet_ = 1000;        // overriden by yaml
    unsigned maxOdds_    = 5;           // overriden by yaml
    Bank houseBank_;                    // overriden by yaml
    Dice dice_;
    unsigned point_ = 0;
    Gen::Uuid currentShooterId_;
    bool bettingOpen_ = true;
    bool isTestRoll_ = false;
    Dice testRollDice_;
    TableStats currentStats_;
    TableStats alltimeStats_;
    std::deque<Dice> recentRolls_;  // Roll history. Front element is oldest roll
    size_t recentRollsMaxSize_ = 25;

    CrapsTable();  // private ctor
    
    static constexpr unsigned InitialStartingBankBalance_ = 3000000;
    static constexpr unsigned RefillThreshold_            = 1000000;
    static constexpr unsigned RefillAmount_               = 2000000;
    
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
    using BetList = std::list<BetPtr>;
    using BetTable = std::array<BetList, EnumBetName::enumerators.size()>;
    BetTable tableBets_;

    // After each dice roll, a decision list is populated with the
    // the results of all bets on the table, one entry for each bet.
    using DecisionList = std::list<DecisionRecord>;
    DecisionList drl_;

    void removePlayerBets(const Gen::Uuid& playerId);
    void removeBetsByPlayerId(BetList& bets, const Gen::Uuid& playerId);

    // Turn bet name enums into size_t to avoid casting each time.
    // Used when directly indexing into tableBets_;
    // TODO might not need this - remove later.
    static inline constexpr size_t PlaceBetIndex = static_cast<size_t>(BetName::Place);

    bool betAllowed(CrapsBet& bet, Gen::ErrorPass& ep) const;
    void declareBettingClosed();
    void throwDice();
    void resolveBets();
    void advanceState();
    void advanceShooter();
    void bumpRecentRolls(const Dice& dice);
    void declareBettingOpen();
    void evaluateBets();
    void dispenseResults();
    void trimTableBets();
    void clearDrl();
    void evalOneBet(const BetPtr pBet);
    bool removeMatchingBetId(BetList& bets, unsigned betId);
    CrapsBet* findBetById(unsigned betId) const;
    std::string diagLimits(Gen::Money amt) const;

    void disburseHouseResults();
    void disbursePlayerWins();
    void disbursePlayerLoses();
    void disbursePlayerKeeps();

    void saveFile(const std::string& dir) const;
    void loadFile(const std::string& dir);
    void setMaxSessions();
    void setHouseBank  ();

    YAML::Node toYAML() const;
    YAML::Node rulesToYAML() const;
    void fromYAML(const YAML::Node& node);
    void rulesFromYAML(const YAML::Node& node);
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

} // namespace Craps

//----------------------------------------------------------------
