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
#include <craps/CrapsBetIntfc.h>
#include <craps/OddsTables.h>
#include <gen/MoneyUtil.h>
#include <gen/ReturnCode.h>

namespace Gen {
    class ErrorPass;  // fwd
}

namespace Craps {

class Dice;            // fwd
class DecisionRecord;  // fwd
    
class CrapsBet : public CrapsBetIntfc
{
public:
    /// @name Lifecycle
    /// @{
    CrapsBet(const Gen::Uuid& playerId, BetName name,
             Gen::Money contractAmount, unsigned pivot = 0);
    /// @}

    /// @name Modifiers
    /// @{
    void setOffComeOutRoll() override;  // intfc
    void setOnComeOutRoll()  override;  // intfc
    void setHardwayOff()     override;  // intfc
    void setHardwayOn()      override;  // intfc
    
    Gen::ReturnCode setContractAmount(Gen::Money amount, Gen::ErrorPass& ep);
    Gen::ReturnCode setOddsAmount    (Gen::Money amount, Gen::ErrorPass& ep);
    
    Gen::ReturnCode evaluate(unsigned point, const Dice& dice,
                             DecisionRecord& dr, Gen::ErrorPass& ep);
    /// @}

    /// @name Observers
    /// @{
    const Gen::Uuid& playerId()  const override;  // intfc
    unsigned betId()             const override;  // intfc
    BetName betName()            const override;  // intfc
    unsigned pivot()             const override;  // intfc
    unsigned contractAmount()    const override;  // intfc
    unsigned oddsAmount()        const override;  // intfc
    bool offComeOutRoll()        const override;  // intfc
    bool hardwayWorking()        const override;  // intfc
    unsigned distance()          const override;  // intfc
    Gen::Timepoint whenCreated() const override;  // intfc
    Gen::Timepoint whenDecided() const override;  // intfc
    
    bool operator==(const CrapsBet&) const;
    /// @}

private:
    enum Decision
    {
        Win,
        Lose,
        Keep
    };

    static unsigned idCounter_;
    void validArgsCtor();
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

    Gen::Uuid playerId_;
    unsigned betId_ = 0;
    BetName betName_ = BetName::Invalid;
    unsigned pivot_ = 0;
    Gen::Money contractAmount_ = 0;
    Gen::Money oddsAmount_ = 0;
    bool offComeOutRoll_ = true;
    unsigned distance_ = 0;  // num rolls until decision
    Gen::Timepoint whenCreated_;
    Gen::Timepoint whenDecided_;

    static const std::unordered_set<unsigned> pointNums_;
    static const std::unordered_set<unsigned> fieldNums_;
    static const std::unordered_set<unsigned> crapsNums_;
    static const std::unordered_set<unsigned> bookEnds_;
    static const std::unordered_set<unsigned> hardwayNums_;

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
