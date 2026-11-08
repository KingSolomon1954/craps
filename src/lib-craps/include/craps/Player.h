//----------------------------------------------------------------
//
// File: Player.h
//
//----------------------------------------------------------------

#pragma once

#include <craps/Bank.h>
#include <craps/CrapsBet.h>
#include <craps/CrapsTypes.h>
#include <craps/EnumBetName.h>
#include <craps/LastRollStats.h>
#include <craps/PlayerStats.h>
#include <gen/ReturnCode.h>
#include <gen/ErrorPass.h>
#include <gen/MoneyUtils.h>
#include <yaml-cpp/yaml.h>
#include <zeus/expected.hpp>
#include <string>
#include <memory>
#include <list>

namespace Gen {
    class ErrorPass;     // fwd
}

namespace Ctrl {
    struct UslDiceRollValue;
}

namespace Craps {

class DecisionRecord;   // fwd
class CrapsTable;       // fwd

class Player
{
public:
    using Bets = std::vector<BetPtr>;

    /// @name Lifecycle
    /// @{
    explicit Player(const PlayerId& playerId); // Existing playerId, rest
                                               // comes from fromYAML()
   ~Player() = default;
    void shutdown();
    void endSession();

    static Player* createPlayer(const std::string& playerName); // Fresh PlayerId
    static Player* fromString  (const std::string& yaml,
                                const PlayerId&    playerId);
    /// @}

    /// What makeOrChangeBet() did
    struct BetPlaced
    {
        BetPtr     bet;                    ///< The bet now on the table
        bool       changed = false;        ///< true if it was already there
        Gen::Money previousAmount = 0;     ///< Contract amount before, if changed
    };

    /// @name Modifiers
    /// @{
    zeus::expected<BetPtr, Gen::ErrorPass> makeBet(
        BetName betName, Gen::Money contractAmount, unsigned pivot);
    zeus::expected<BetPlaced, Gen::ErrorPass> makeOrChangeBet(
        BetName betName, Gen::Money contractAmount, unsigned pivot);
    zeus::expected<void, Gen::ErrorPass> joinTable(CrapsTable& table);
    zeus::expected<void, Gen::ErrorPass> leaveTable();
    zeus::expected<void, Gen::ErrorPass> setContractAmount(
        BetPtr pBet, Gen::Money newAmount);
    zeus::expected<void, Gen::ErrorPass> setOddsAmount(
        BetPtr pBet, Gen::Money oddsAmount);
    zeus::expected<void, Gen::ErrorPass> removeBet(const BetId& betId);

    Gen::ReturnCode restoreBet(BetPtr pBet, Gen::ErrorPass& ep);
    void restoreAmounts(BetPtr pBet, const CrapsBet& prevState);
    void bettingClosed();
    void disburseDone();
    void processWin (const DecisionRecord& dr);
    void processLose(const DecisionRecord& dr);
    void processKeep(const DecisionRecord& dr);
    void resetStats();
    /// @}

    /// @name Observers
    /// @{
    const std::string&   getName()                                const;
    const PlayerId&      getPlayerId()                            const;
    Gen::Money           getAmountOnTable()                       const;
    unsigned             getNumBetsOnTable()                      const;
    Gen::Money           getBalance()                             const;
    int                  getSessionNet()                          const;
    BetPtr               getBet(const BetId& betId, Gen::ErrorPass& ep) const;
    Craps::BetId         getBet(BetName betName, size_t pivot, Gen::ErrorPass& ep) const;
    BetPtr               findBet(BetName betName, unsigned pivot) const;
    Bets                 getBets()                                const;
    Bets                 getOddsBets()                            const;

    const LastRollStats& getLastRollStats()                       const;
    const PlayerStats&   getCurrentStats()                        const;
    const PlayerStats&   getAlltimeStats()                        const;
    const BankStats&     getBankCurrentStats()                    const;
    const BankStats&     getBankAlltimeStats()                    const;
    bool                 haveBet(const BetId& betId)              const;
    bool                 haveAnyOddsCapableBet()                  const;
    const SessionHistory::Sessions& getSessionHistory()           const;

    /// @name YAML conversion
    /// @{
    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);   // throws
    /// @}

private:
    void beginNewSession();
    // order matters
    PlayerId          playerId_;
    Bank              wallet_;     // overriden by yaml

    // order doesn't matter
    std::string       playerName_; // set by yaml or creation
    std::string       shortDescription_;
    std::string       fullDescription_;
    std::list<BetPtr> bets_;
    CrapsTable*       pTable_ = nullptr;
    bool              sessionEnded_ = false; // endSession() ran; the next
                                             // joinTable() starts a new one
    PlayerStats       currentStats_;
    PlayerStats       alltimeStats_;
    mutable LastRollStats lastRollStats_;

    // Default bank constants for player
    static constexpr unsigned InitialStartingBankBalance_ = 30000;
    static constexpr unsigned RefillThreshold_            = 15000;
    static constexpr unsigned RefillAmount_               = 20000;

private:
    BetPtr makeShared(BetName betName,
                      Gen::Money contractAmount,
                      unsigned pivot,
                      Gen::ErrorPass& ep);
    BetPtr findBetById(const BetId& betId) const;
    bool removeBetByPtr(BetPtr& pBet);
    void setName(const std::string& playerName);
    void sendEventBetResolved(BetName betName,
                              BetId betId,
                              const DecisionRecord& dr) const;
    void sendEventBetResolved(BetName         betName,
                              BetId           betId,
                              Gen::Money      amountWin,
                              Gen::Money      amountLose) const;
    void sendEventPlayerBetsChanged()                     const;
    void sendEventPlayerSessionStatsChanged()             const;
    void sendEventPlayerLastRollStatsChanged()            const;
    void sendEventPlayerBalanceChanged()                  const;

    // Validity checks with diagnostics
    void diagBadBetId(const std::string& funcName, BetId betId) const;
    std::string diagPrefix(size_t idx)                          const;
    bool fifNoTable(Gen::ErrorPass& ep)                         const;
    bool fifMissingBet(BetPtr pBet, Gen::ErrorPass& ep)         const;
    bool fifInsufficientFunds(BetPtr pBet,
                              Gen::Money newAmount,
                              Gen::Money oldAmount,
                              Gen::ErrorPass& ep)               const;
    bool fifBadAddBet (BetPtr pBet, Gen::ErrorPass& ep)         const;

    using En = Gen::ErrorPass::ErrorName;
    using Et = Gen::ErrorPass::ErrorType;
    using Es = Gen::ErrorPass::Severity;
};

/*-----------------------------------------------------------*//**

@class Player

@brief Craps player

Player Responsibilities:

@li Maintains player's money
@li Tracks player's bets
@li A player has at most one bet per spot, as at a real table where the
    chips stack up. makeOrChangeBet() makes a bet, or if the player
    already has that bet changes its amount to the amount given.
    makeBet() always makes a new bet.
@li Exports its own state with toYAML() and restores it with fromYAML()
@li Knows nothing about files, paths or where that YAML is stored.
    Lib-controller's PlayerManager owns each player's file and decides
    when to read and write it.
@li endSession() ends the player's session: leaves the table, records the
    session summary, merges stats. It writes nothing to disk. Call
    toYAML() and persist it afterward. The player can then join a table
    again, which starts a fresh session (new stats, new session clock,
    the balance carries over). shutdown() is endSession() at program
    quit.

*/

} // namespace Craps

//----------------------------------------------------------------
