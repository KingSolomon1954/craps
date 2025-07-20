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

class CrapsTable;      //fwd
class Dice;            // fwd
class DecisionRecord;  // fwd
    
class CrapsBet
{
public:
    /// @name Lifecycle
    /// @{
    CrapsBet(const Gen::Uuid& playerId, BetName name,
             Gen::Money contractAmount, unsigned pivot = 0);
    /// @}

    /// @name Modifiers
    /// @{
    Gen::ReturnCode changeContractAmount(Gen::Money amount, Gen::ErrorPass& ep);
    Gen::ReturnCode changeOddsAmount    (Gen::Money amount, Gen::ErrorPass& ep);
    void setOffComeOutRoll();
    void setOnComeOutRoll();
    void setHardwayOff();
    void setHardwayOn();
    
    Gen::ReturnCode evaluate(unsigned point, const Dice& dice,
                             DecisionRecord& dr, Gen::ErrorPass& ep);
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
    Gen::Timepoint whenCreated_;
    Gen::Timepoint whenDecided_;
    CrapsTable* pTable_;
    
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

    void setCrapsTablePtr(CrapsTable* pTable);
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

    std::string coaPrefix()                      const;
    bool coaCheckBetType    (Gen::ErrorPass& ep) const;
    bool coaCheckNoTable    (Gen::ErrorPass& ep) const;
    bool coaCheckBettingOpen(Gen::ErrorPass& ep) const;
    bool coaCheckPivot      (Gen::ErrorPass& ep) const;
    bool coaCheckTooSmall   (Gen::Money newAmount, Gen::ErrorPass& ep) const;
    bool coaCheckMaxOdds    (Gen::Money newAmount, Gen::ErrorPass& ep) const;
    std::string diagTooSmall(Gen::Money amount, Gen::Money min,
                             BetName betName, unsigned pivot) const;

    friend class CrapsTable;
    friend class TableStats;
};

/*-----------------------------------------------------------*//**

@class CrapsBet Implementation

@brief A bet on the craps table.

Manage a craps bet on the table.

*/

} // namespace Craps

std::ostream& operator<< (std::ostream& out, const Craps::CrapsBet& b);
    
//----------------------------------------------------------------
