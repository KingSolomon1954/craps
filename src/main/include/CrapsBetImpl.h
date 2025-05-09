//----------------------------------------------------------------
//
// File: CrapsBetImpl.h
//
//----------------------------------------------------------------

#pragma once

#include <chrono>
#include <ostream>
#include <string>
#include <unordered_set>
#include "gen/ReturnCode.h"
#include "gen/Uuid.h"
#include "DecisionRecord.h"
#include "EnumBetName.h"
#include "Globals.h"
#include "OddsTables.h"

namespace Gen {
    class ErrorPass;  // fwd
}

namespace App {

class Dice;  // fwd
    
class CrapsBetImpl : public CrapsBet
{
public:
    /// @name Lifecycle
    /// @{
    CrapsBet(const Gen::Uuid& playerId, BetName name,
             Money contractAmount, unsigned pivot = 0);
    /// @}

    /// @name Modifiers
    /// @{
    Gen::ReturnCode evaluate(unsigned point, const Dice& dice,
                             DecisionRecord& dr, Gen::ErrorPass& ep);
    Gen::ReturnCode setOddsAmount(Money amount, Gen::ErrorPass& ep);

    
    void setOffComeOutRoll() override;
    void setOnComeOutRoll() override;
    void setHardwayOff() override;
    void setHardwayOn() override;
    /// @}

    /// @name Observers
    /// @{
    const Gen::Uuid& playerId() const override;
    unsigned betId() const override;
    BetName betName() const override;
    unsigned pivot() const override;
    unsigned contractAmount() const override;
    unsigned oddsAmount() const override;
    bool offComeOutRoll() const override;
    bool hardwayWorking() const override;
    unsigned distance() const override;
    std::chrono::time_point<std::chrono::system_clock> whenCreated() const override;
    std::chrono::time_point<std::chrono::system_clock> whenDecided() const override;
    
    bool operator==(const CrapsBet&) const = default;
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
    bool skip_ = false;
    unsigned distance_ = 0;  // num rolls until decision
    std::chrono::time_point<std::chrono::system_clock> whenCreated_;
    std::chrono::time_point<std::chrono::system_clock> whenDecided_;

    std::unordered_set<unsigned> pointNums_   = {4, 5, 6, 8, 9, 10};
    std::unordered_set<unsigned> fieldNums_   = {2, 3, 4, 9, 10, 11, 12};
    std::unordered_set<unsigned> crapsNums_   = {2, 3, 12};
    std::unordered_set<unsigned> bookEnds_    = {2, 3, 11, 12};
    std::unordered_set<unsigned> hardwayNums_ = {4, 6, 8, 10};
};

/*-----------------------------------------------------------*//**

@class CrapsBet

@brief A bet on the craps table.

Store info that represents a craps bet.

*/

} // namespace App

friend std::ostream& operator<< (std::ostream& out, const App::CrapsBet& b);
    
//----------------------------------------------------------------
