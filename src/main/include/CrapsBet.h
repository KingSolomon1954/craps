//----------------------------------------------------------------
//
// File: CrapsBet.h
//
//----------------------------------------------------------------

#pragma once

#include <chrono>
#include "Globals.h"
#include "EnumBetName.h"

namespace App {

class Dice;  // fwd
    
class CrapsBet
{
public:
    /// @name Lifecycle
    /// @{
    CrapsBet(BetName name, Money contractAmount, unsigned pivot = 0);
    CrapsBet(const CrapsBet& other) = default;  // copy ctor
    CrapsBet(CrapsBet&& other) = default;  // move ctor
    CrapsBet& operator=(const CrapsBet&) = default;  // assignment
    CrapsBet& operator=(CrapsBet&&) = default;  // assignment
   ~CrapsBet() = default;
    
    /// @}

    /// @name Modifiers
    /// @{
    bool evaluate(unsigned point, const Dice& dice);
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

struct DecisionRecord
{
    unsigned betId;
    bool decision = false;
    Money win = 0;
    Money lose = 0;
    Money returnToPlayer = 0;
    
    DecisionRecord(unsigned id)
        : betId(id)
        , decision(false)
        , win(0)
        , lose(0)
        , returnToPlayer(0)
    {}
};
    
private:
    enum Decision
    {
        Win,
        Lose,
        Keep
    };

    static unsigned idCounter_;
    void validArgsCtor();
    bool validArgsEval();
    bool evalPassLine(unsigned point, const Dice& dice);

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

//----------------------------------------------------------------
