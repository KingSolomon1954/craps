//----------------------------------------------------------------
//
// File: CrapsBet.h
//
//----------------------------------------------------------------

#pragma once

#include <chrono>
#include <ostream>
#include <unordered_set>
// #include <variant>
#include "gen/ReturnCode.h"
#include "Globals.h"
#include "EnumBetName.h"
#include "OddsTables.h"

namespace Gen {
    class ErrorPass;  // fwd
}

namespace App {

class Dice;  // fwd
    
class CrapsBet
{
public:
    /// @name Lifecycle
    /// @{
    CrapsBet(BetName name, Money contractAmount, unsigned pivot = 0);
    /// @}

    /// @name Modifiers
    /// @{
    struct DecisionRecord
    {
        unsigned betId = 0;
        bool decision = false;
        bool pivotAssigned = false;
        Money win = 0;
        Money lose = 0;
        Money returnToPlayer = 0;

        bool operator==(const DecisionRecord& other) const
        {
            return betId == other.betId &&
                   decision == other.decision &&
                   pivotAssigned == other.pivotAssigned &&
                   win == other.win &&
                   lose == other.lose &&
                   returnToPlayer == other.returnToPlayer;
        }
    };
    Gen::ReturnCode evaluate(unsigned point, const Dice& dice,
                             DecisionRecord& dr, Gen::ErrorPass& ep);
    Gen::ReturnCode setOddsAmount(Money amount, Gen::ErrorPass& ep);
    void setOddsBetWorkingComeOutRoll();
    void setOddsBetOffComeOutRoll();
    void setPlaceBetWorkingComeOutRoll();
    void setPlaceBetOffComeOutRoll();
    void setBuyBetWorkingComeOutRoll();
    void setBuyBetOffComeOutRoll();
    void setLayBetWorkingComeOutRoll();
    void setLayBetOffComeOutRoll();
    void setHardwayBetWorking();
    void setHardwayBetOff();
    /// @}

    /// @name Observers
    /// @{
    unsigned betId() const;
    BetName betName() const;
    unsigned pivot() const;
    unsigned contractAmount() const;
    unsigned oddsAmount() const;
    bool oddsBetOffComeOutRoll() const;
    bool placeBetOffComeOutRoll() const;
    bool layBetOffComeOutRoll() const;
    bool hardwayBetOff() const;
    unsigned distance() const;
    std::chrono::time_point<std::chrono::system_clock> whenCreated() const;
    std::chrono::time_point<std::chrono::system_clock> whenDecided() const;
    
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
    Gen::ReturnCode evalBuy(
        unsigned point, const Dice& dice,
        DecisionRecord& dr, Gen::ErrorPass& ep);
    Gen::ReturnCode evalLay(
        unsigned point, const Dice& dice,
        DecisionRecord& dr, Gen::ErrorPass& ep);
    Gen::ReturnCode evalField(
        unsigned point, const Dice& dice,
        DecisionRecord& dr, Gen::ErrorPass& ep);
    Gen::ReturnCode evalHardway(
        unsigned point, const Dice& dice,
        DecisionRecord& dr, Gen::ErrorPass& ep);
    void diagEvalEntered(unsigned point, const Dice& dice) const;
    Gen::ReturnCode diagEvalProcError(Gen::ErrorPass& ep) const;
    void calcWinPointBet(unsigned diceVal,
                         DecisionRecord& dr,
                         bool returnOdds,
                         const OddsTables::OddsEntry table[]) const;
    void calcLossAnyBet(DecisionRecord& dr, bool returnOdds) const;

    
    unsigned betId_ = 0;
    BetName betName_ = BetName::Invalid;
    unsigned pivot_ = 0;
    Money contractAmount_ = 0;
    Money oddsAmount_ = 0;
    bool oddsBetOffComeOutRoll_ = true;
    bool placeBetOffComeOutRoll_ = true;
    bool buyBetOffComeOutRoll_ = true;
    bool layBetOffComeOutRoll_ = true;
    bool hardwayBetOff_ = false;
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

std::ostream& operator<< (std::ostream& out, const App::CrapsBet& b);
std::ostream& operator<< (std::ostream& out, const App::CrapsBet::DecisionRecord& dr);
    
//----------------------------------------------------------------
