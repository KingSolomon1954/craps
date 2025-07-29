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

class Dice;            // fwd
class DecisionRecord;  // fwd
    
class CrapsBet
{
public:
    using BetPtr = std::shared_ptr<class CrapsBet>;
    
    /// @name Lifecycle
    /// @{
    CrapsBet(const Gen::Uuid& playerId,
             BetName          name,
             Gen::Money       contractAmount,
             unsigned         pivot = 0);
    /// @}

    /// @name Modifiers
    /// @{
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
    std::string      diagBetId()      const;  // "bet(betId:33, betName:DontCome(4))"
    
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
    unsigned calculateOddsPayout(unsigned amount,
                                 unsigned numerator,
                                 unsigned denominator) const;
    void calcWinPointBet(unsigned diceVal,
                         DecisionRecord& dr,
                         bool returnOdds,
                         const OddsTables::OddsEntry table[]) const;
    void calcLossPointBet(DecisionRecord& dr, bool returnOdds) const;
    void setContractAmountInternal(Gen::Money amount); // called by CrapsTable
    void setOddsAmountInternal    (Gen::Money amount); // called by CrapsTable
    void setPivotInternal         (unsigned pivot);    // called by CrapsTable

    friend class CrapsTable;
    friend class TableStats;

    // Unit test access points
#ifdef UNIT_TEST
public:
    void testSetContractAmount(Gen::Money amount)
    { setContractAmountInternal(amount); }
    void testSetOddsAmount(Gen::Money amount)
    { setOddsAmountInternal(amount); }
    void testSetPivot(unsigned pivot)
    { setPivotInternal(pivot); }
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
