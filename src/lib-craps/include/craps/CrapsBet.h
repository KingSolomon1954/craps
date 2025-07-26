//----------------------------------------------------------------
//
// File: CrapsBet.h
//
//----------------------------------------------------------------

#pragma once

#include <chrono>
#include <ostream>
#include <string>
#include <unordered_set>
#include <craps/OddsTables.h>
#include <craps/EnumBetName.h>
#include <gen/MoneyUtil.h>
#include <gen/ReturnCode.h>
#include <gen/Timepoint.h>
#include <gen/Uuid.h>

namespace Gen {
    class ErrorPass;  // fwd
}

namespace Craps {

class CrapsTable;      // fwd
class Dice;            // fwd
class DecisionRecord;  // fwd
    
class CrapsBet
{
public:
    using BetPtr = std::shared_ptr<class CrapsBet>;
    
    /// @name Lifecycle
    /// @{
    CrapsBet(const Gen::Uuid& playerId, BetName name,
             Gen::Money contractAmount, unsigned pivot = 0);
    /// @}

    /// @name Modifiers
    /// @{
    Gen::ReturnCode setContractAmount(Gen::Money amount, Gen::ErrorPass& ep);
    Gen::ReturnCode setOddsAmount    (Gen::Money amount, Gen::ErrorPass& ep);
    void setOffComeOutRoll();
    void setOnComeOutRoll();
    void setHardwayOff();
    void setHardwayOn();
    /// @}

    /// @name Observers
    /// @{
    const Gen::Uuid& playerId()       const;
    unsigned         betId()          const;
    BetName          betName()        const;
    unsigned         pivot()          const;
    unsigned         contractAmount() const;
    unsigned         oddsAmount()     const;
    bool             offComeOutRoll() const;
    bool             hardwayWorking() const;
    unsigned         distance()       const;
    Gen::Timepoint   whenCreated()    const;
    Gen::Timepoint   whenDecided()    const;
    
    bool operator==(const CrapsBet&) const;
    /// @}

private:
    // Order matters, in initializer list
    Gen::Uuid playerId_;
    unsigned betId_            = 0;
    BetName betName_           = BetName::Invalid;
    unsigned pivot_            = 0;
    Gen::Money contractAmount_ = 0;

    // Order doesn't matter
    Gen::Money oddsAmount_     = 0;
    bool offComeOutRoll_       = true;
    unsigned distance_         = 0;  // num rolls until decision
    CrapsTable* pTable_        = nullptr;
    Gen::Timepoint whenCreated_;
    Gen::Timepoint whenDecided_;
    
    static unsigned idCounter_;
    static const std::unordered_set<unsigned> pointNums_;
    static const std::unordered_set<unsigned> fieldNums_;
    static const std::unordered_set<unsigned> crapsNums_;
    static const std::unordered_set<unsigned> bookEnds_;
    static const std::unordered_set<unsigned> hardwayNums_;

    enum Decision
    {
        Win,
        Lose,
        Keep
    };

    Gen::ReturnCode evaluate(unsigned point, const Dice& dice,
                             DecisionRecord& dr, Gen::ErrorPass& ep);
    void attachCrapsTable(CrapsTable* pTable);
    void detachCrapsTable(CrapsTable* pTable);
    void checkBetName();
    void checkContractAmount();
    void checkLinePivot();
    void checkPlacePivot();
    void checkHardwayPivot();
    void checkSideBets();
    bool validArgsEval(unsigned point, Gen::ErrorPass& ep) const;
    Gen::ReturnCode evalPassLine(
        unsigned point, const Dice& dice,
        DecisionRecord& dr, Gen::ErrorPass& ep);
    Gen::ReturnCode evalCome(
        unsigned point, const Dice& dice,
        DecisionRecord& dr, Gen::ErrorPass& ep);
    Gen::ReturnCode evalDontPass(
        unsigned point, const Dice& dice,
        DecisionRecord& dr, Gen::ErrorPass& ep);
    Gen::ReturnCode evalDontCome(
        unsigned point, const Dice& dice,
        DecisionRecord& dr, Gen::ErrorPass& ep);
    Gen::ReturnCode evalPlace(
        unsigned point, const Dice& dice,
        DecisionRecord& dr, Gen::ErrorPass& ep);
    Gen::ReturnCode evalLay(
        unsigned point, const Dice& dice,
        DecisionRecord& dr, Gen::ErrorPass& ep);
    Gen::ReturnCode evalBuy(
        unsigned point, const Dice& dice,
        DecisionRecord& dr, Gen::ErrorPass& ep);
    Gen::ReturnCode evalHardway(
        unsigned point, const Dice& dice,
        DecisionRecord& dr, Gen::ErrorPass& ep);
    Gen::ReturnCode evalField(
        unsigned point, const Dice& dice,
        DecisionRecord& dr, Gen::ErrorPass& ep);
    Gen::ReturnCode evalAnyCraps(
        unsigned point, const Dice& dice,
        DecisionRecord& dr, Gen::ErrorPass& ep);
    Gen::ReturnCode evalAnySeven(
        unsigned point, const Dice& dice,
        DecisionRecord& dr, Gen::ErrorPass& ep);
    Gen::ReturnCode evalCandE(
        unsigned point, const Dice& dice,
        DecisionRecord& dr, Gen::ErrorPass& ep);
    Gen::ReturnCode evalHorn(
        unsigned point, const Dice& dice,
        DecisionRecord& dr, Gen::ErrorPass& ep);
    void diagEvalEntered(unsigned point, const Dice& dice) const;
    Gen::ReturnCode diagEvalProcError(Gen::ErrorPass& ep) const;
    void calcWinPointBet(unsigned diceVal,
                         DecisionRecord& dr,
                         bool returnOdds,
                         const OddsTables::OddsEntry table[]) const;
    void calcLossPointBet(DecisionRecord& dr, bool returnOdds) const;

    std::string scaPrefix() const;
    std::string soaPrefix() const;
    bool scaCheckZero(          Gen::Money amount, Gen::ErrorPass& ep)  const;
    bool scaCheckPassLineChange(Gen::Money amount, Gen::ErrorPass& ep)  const;
    bool scaCheckDontPassChange(Gen::Money amount, Gen::ErrorPass& ep)  const;
    bool scaCheckTableLimit    (Gen::Money amount, Gen::ErrorPass& ep)  const;
    bool soaCheckBetType       (Gen::ErrorPass& ep) const;
    bool soaCheckNoTable       (Gen::ErrorPass& ep) const;
    bool soaCheckBettingOpen   (Gen::ErrorPass& ep) const;
    bool soaCheckHavePivot     (Gen::ErrorPass& ep) const;
    bool soaCheckTooSmall      (Gen::Money newAmount, Gen::ErrorPass& ep) const;
    bool soaCheckMaxOdds       (Gen::Money newAmount, Gen::ErrorPass& ep) const;
    std::string diagTooSmall   (Gen::Money amount, Gen::Money min,
                             BetName betName, unsigned pivot) const;
    std::string diagCurrentBet() const;

    friend class CrapsTable;
    friend class TableStats;

    // Unit test access points
#ifdef UNIT_TEST
public:
    void testAttachCrapsTable(CrapsTable* pTable)
    { attachCrapsTable(pTable); }
    void testDetachCrapsTable(CrapsTable* pTable)
    { detachCrapsTable(pTable); }
    Gen::ReturnCode testEvaluate(unsigned point, const Dice& dice,
                                 DecisionRecord& dr, Gen::ErrorPass& ep)
    { return evaluate(point, dice, dr, ep); }
#endif
    
};  // class CrapsBet

/*-----------------------------------------------------------*//**

@class CrapsBet Implementation

@brief A bet on the craps table.

Manage a craps bet on the table.

*/

} // namespace Craps

std::ostream& operator<< (std::ostream& out, const Craps::CrapsBet& b);
    
//----------------------------------------------------------------
