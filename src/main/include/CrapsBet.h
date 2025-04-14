//----------------------------------------------------------------
//
// File: CrapsBet.h
//
//----------------------------------------------------------------

#pragma once

#include <chrono>
#include <ostream>
// #include <variant>
#include "gen/ReturnCode.h"
#include "Globals.h"
#include "EnumBetName.h"

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
        Money win = 0;
        Money lose = 0;
        Money returnToPlayer = 0;
    };
    Gen::ReturnCode evaluate(unsigned point, const Dice& dice,
                             DecisionRecord& dr, Gen::ErrorPass& ep);
    /// @}

    /// @name Observers
    /// @{
    unsigned betId() const;
    BetName betName() const;
    unsigned pivot() const;
    unsigned contractAmount() const;
    unsigned oddsAmount() const;
    bool oddsOffComeOutRoll() const;
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
    void diagEvalEntered(unsigned point, const Dice& dice) const;
    Gen::ReturnCode diagEvalProcError(Gen::ErrorPass& ep) const;

    unsigned betId_ = 0;
    BetName betName_ = BetName::Invalid;
    unsigned pivot_ = 0;
    Money contractAmount_ = 0;
    Money oddsAmount_ = 0;
    bool oddsOffComeOutRoll_ = true;
    unsigned distance_ = 0;  // num rolls until decision
    std::chrono::time_point<std::chrono::system_clock> whenCreated_;
    std::chrono::time_point<std::chrono::system_clock> whenDecided_;
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
