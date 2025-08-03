//----------------------------------------------------------------
//
// File: CrapsTable.h
//
//----------------------------------------------------------------

#pragma once

#include <array>
#include <deque>
#include <list>
#include <gen/ErrorPass.h>
#include <gen/MoneyUtil.h>
#include <gen/ReturnCode.h>
#include <gen/Timepoint.h>
#include <craps/Bank.h>
#include <craps/CrapsBet.h>
#include <craps/CrapsTypes.h>
#include <craps/DecisionRecord.h>
#include <craps/Dice.h>
#include <craps/EventManager.h>
#include <craps/Player.h>
#include <craps/TableConfig.h>
#include <craps/TableStats.h>
#include <yaml-cpp/yaml.h>

namespace Craps {

class CrapsTable
{
public:
    /// @name Lifecycle
    /// @{
    CrapsTable(const TableId&     tableId,
               const TableConfig& config,
               EventManager&      eventMgr);
   ~CrapsTable() = default;
    static CrapsTable* fromConfig(const TableId&     tableId,
                                  const TableConfig& config,
                                  EventManager&      eventMgr);
    static CrapsTable* fromFile  (const TableId&     tableId,
                                  const TableConfig& config,
                                  EventManager&      eventMgr);
    /// @}

    /// @name Modifiers
    /// @{
    // Players
    Gen::ReturnCode addPlayer   (Player* pPlayer, Gen::ErrorPass& ep);
    Gen::ReturnCode removePlayer(Player* pPlayer, Gen::ErrorPass& ep);

    // Bets
    Gen::ReturnCode addBet   (BetPtr pBet, Gen::ErrorPass& ep);
    Gen::ReturnCode removeBet(BetPtr pBet, Gen::ErrorPass& ep);
    Gen::ReturnCode setContractAmount(BetPtr pBet,
                                      Gen::Money newAmount,
                                      Gen::ErrorPass& ep);
    Gen::ReturnCode setOddsAmount(BetPtr pBet,
                                  Gen::Money oddsAmount,
                                  Gen::ErrorPass& ep);
    // Table
    void rollDice();
    void resetStats();
    void close();               // Shutdown table, switching to different table
    void prepareForShutdown();  // Shutdown table, exiting program
    /// @}

    /// @name Observers
    /// @{
    unsigned                getNumPlayers()       const;
    unsigned                getPoint()            const;
    Dice                    getCurrentRoll()      const;
    Gen::Money              getAmountOnTable()    const;
    unsigned                getNumBetsOnTable()   const;
    std::vector<Player*>    getPlayers()          const;
    Player*                 getCurrentShooter()   const;
    const std::deque<Dice>& getRecentRolls()      const;
    const TableStats&       getCurrentStats()     const;
    const TableStats&       getAlltimeStats()     const;
    const BankStats&        getBankCurrentStats() const;
    const BankStats&        getBankAlltimeStats() const;
    unsigned                getMinLineBet()       const;
    unsigned                getMaxLineBet()       const;
    unsigned                getMaxOdds()          const;
    bool                    isComeOutRoll()       const;
    bool                    isBettingOpen()       const;
    bool                    havePlayer(Player* pPlayer) const;
    bool                    haveBet(const CrapsBet& bet) const;
    const SessionHistory::Sessions& getSessionHistory() const;
    /// @}

#if 0
    void resetTable();
    std::vector<CrapsBet> getBetsForPlayer(const std::string& playerName) const;

#endif
    // Unit test access points
#ifdef UNIT_TEST
public:
    void testRollDice(unsigned d1, unsigned d2)
    {
        isTestRoll_ = true;
        testRollDice_.set(d1, d2);
        rollDice();
    };
    void testSetState(unsigned point, unsigned d1, unsigned d2)
    {
        point_ = point;
        dice_.set(d1, d2);
    }
#endif

private:
    TableId tableId_;
    TableConfig config_;
    EventManager& eventMgr_;
    std::string tableName_;
    std::string shortDescription_;
    std::string fullDescription_;
    unsigned maxOdds_     = 5;
    unsigned minLineBet_  = 5;
    unsigned maxLineBet_  = 1000;
    unsigned minPlaceBet_ = 5;
    unsigned maxPlaceBet_ = 1000;
    unsigned minFieldBet_ = 1;
    unsigned maxFieldBet_ = 1000;
    unsigned minCandEBet_ = 2;
    unsigned maxCandEBet_ = 1000;
    unsigned minHornBet_  = 4;
    unsigned maxHornBet_  = 1000;
    Bank houseBank_;  // overriden by yaml
    Dice dice_;
    unsigned point_ = 0;
    Player* pCurrentShooter_ = nullptr;
    bool bettingOpen_ = true;
    bool isTestRoll_ = false;
    Dice testRollDice_;
    TableStats currentStats_;
    TableStats alltimeStats_;
    std::deque<Dice> recentRolls_;  // Front element is oldest roll

    CrapsTable();  // private ctor

    // Players must join table in order to play.
    static inline constexpr size_t MaxPlayers = 6;
    using PlayerList = std::list<Player*>;
    PlayerList players_;

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
    void evalOneBet(CrapsBet& bet);

    Gen::ReturnCode removePlayerByPtr(Player* pPlayer, Gen::ErrorPass& ep);
    void removePlayerBets            (Player* pPlayer);
    void removeBetsByPlayerPtr       (BetList& bets, Player* pPlayer);
    bool removeMatchingBet           (BetList& bets, CrapsBet* pBet);
    BetPtr findBetById               (BetId betId) const;

    void disburseHouseResults();
    void disbursePlayerWins();
    void disbursePlayerLoses();
    void disbursePlayerKeeps();

    // File operations
    void saveFile() const;
    void loadFile();
    YAML::Node toYAML() const;
    YAML::Node rulesToYAML() const;
    void fromYAML(const YAML::Node& node);
    void rulesFromYAML(const YAML::Node& node);

    // Validity checks, in CrapsTablePart2.cpp
    std::string diagPrefix    (size_t idx, const CrapsBet& bet) const;
    bool fifBettingClosed     (const CrapsBet& bet, size_t idx, Gen::ErrorPass& ep) const;
    bool fifMissingBet        (const CrapsBet& bet, size_t idx, Gen::ErrorPass& ep) const;
    bool fifMissingPlayer     (const CrapsBet& bet, Gen::ErrorPass& ep) const;
    bool fifHaveBet           (const CrapsBet& bet, Gen::ErrorPass& ep) const;
    bool fifComeDisallowed    (const CrapsBet& bet, Gen::ErrorPass& ep) const;
    bool fifDontPassDisallowed(const CrapsBet& bet, Gen::ErrorPass& ep) const;
    bool fifBadBetTypeForOdds (const CrapsBet& bet, Gen::ErrorPass& ep) const;
    bool fifZeroPivotForOdds  (const CrapsBet& bet, Gen::ErrorPass& ep) const;
    bool fifBadMinMaxLineBets (const CrapsBet& bet, Gen::Money amt,
                               size_t idx, Gen::ErrorPass& ep) const;
    bool fifBadMinMaxSideBets (const CrapsBet& bet, Gen::Money amt,
                               size_t idx, Gen::ErrorPass& ep) const;
    bool fifZeroAmount        (const CrapsBet& bet, Gen::Money amt,
                               size_t idx, Gen::ErrorPass& ep) const;
    bool fifBadPassLineChange (const CrapsBet& bet, Gen::Money amt,
                               size_t idx, Gen::ErrorPass& ep) const;
    bool fifBadDontPassChange (const CrapsBet& bet, Gen::Money amt,
                               size_t idx, Gen::ErrorPass& ep) const;
    bool fifBadMultiples      (const CrapsBet& bet, Gen::Money amt,
                               size_t idx, Gen::ErrorPass& ep) const;
    bool fifBadMinMaxForOdds  (const CrapsBet& bet, Gen::Money amt,
                               Gen::ErrorPass& ep) const;
    bool withinMinMaxLineBets(BetName betName,
                              Gen::Money contractAmount,
                              Gen::ErrorPass& ep) const;
    bool withinMinMaxSideBets(BetName betName,
                              Gen::Money contractAmount,
                              Gen::ErrorPass& ep) const;
    bool goodMultiplesSideBets(BetName betName,
                              Gen::Money amt,
                              Gen::ErrorPass& ep) const;
    std::string diagLimits   (Gen::Money amt) const;

    // Default bank constants for table
    static constexpr unsigned InitialStartingBankBalance_ = 3000000;
    static constexpr unsigned RefillThreshold_            = 1500000;
    static constexpr unsigned RefillAmount_               = 2000000;
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
