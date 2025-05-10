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
#include "CrapsBetIntfc.h"
#include "gen/ReturnCode.h"
#include "Globals.h"
#include "OddsTables.h"

namespace Gen {
    class ErrorPass;  // fwd
}

namespace App {

class Dice;            // fwd
class DecisionRecord;  // fwd
    
class CrapsBet : public CrapsBetIntfc
{
public:
    /// @name Lifecycle
    /// @{
    CrapsBet(const Gen::Uuid& playerId, BetName name,
             Money contractAmount, unsigned pivot = 0);
    /// @}

    /// @name Modifiers
    /// @{
    void setOffComeOutRoll() override;  // intfc
    void setOnComeOutRoll()  override;  // intfc
    void setHardwayOff()     override;  // intfc
    void setHardwayOn()      override;  // intfc
    
    Gen::ReturnCode setContractAmount(Money amount, Gen::ErrorPass& ep);
    Gen::ReturnCode setOddsAmount    (Money amount, Gen::ErrorPass& ep);
    
    Gen::ReturnCode evaluate(unsigned point, const Dice& dice,
                             DecisionRecord& dr, Gen::ErrorPass& ep);
    /// @}

    /// @name Observers
    /// @{
    const Gen::Uuid& playerId() const override;  // intfc
    unsigned betId()            const override;  // intfc
    BetName betName()           const override;  // intfc
    unsigned pivot()            const override;  // intfc
    unsigned contractAmount()   const override;  // intfc
    unsigned oddsAmount()       const override;  // intfc
    bool offComeOutRoll()       const override;  // intfc
    bool hardwayWorking()       const override;  // intfc
    unsigned distance()         const override;  // intfc
    std::chrono::time_point<std::chrono::system_clock> whenCreated() const override;  // intfc
    std::chrono::time_point<std::chrono::system_clock> whenDecided() const override;  // intfc
    
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
    Money contractAmount_ = 0;
    Money oddsAmount_ = 0;
    bool offComeOutRoll_ = true;
    unsigned distance_ = 0;  // num rolls until decision
    std::chrono::time_point<std::chrono::system_clock> whenCreated_;
    std::chrono::time_point<std::chrono::system_clock> whenDecided_;

    std::unordered_set<unsigned> pointNums_   = {4, 5, 6, 8, 9, 10};
    std::unordered_set<unsigned> fieldNums_   = {2, 3, 4, 9, 10, 11, 12};
    std::unordered_set<unsigned> crapsNums_   = {2, 3, 12};
    std::unordered_set<unsigned> bookEnds_    = {2, 3, 11, 12};
    std::unordered_set<unsigned> hardwayNums_ = {4, 6, 8, 10};

    friend class CrapsTable;
};

/*-----------------------------------------------------------*//**

@class CrapsBet Implementation

@brief A bet on the craps table.

Manage a craps bet on the table.

*/

} // namespace App

std::ostream& operator<< (std::ostream& out, const App::CrapsBet& b);
    
//----------------------------------------------------------------
