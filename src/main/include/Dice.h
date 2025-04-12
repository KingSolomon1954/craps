//----------------------------------------------------------------
//
// File: Dice.h
//
//----------------------------------------------------------------

#pragma once

namespace App {

class Dice
{
public:
    /// @name Lifecycle
    /// @{
    Dice();
   ~Dice() = default;
    /// @}

    /// @name Modifiers
    /// @{
    unsigned roll();
    /// @}

    /// @name Observers
    /// @{
    unsigned value() const;
    unsigned d1() const;
    unsigned d2() const;
    /// @}

private:
    int getRandomNumber(int min, int max) const;
    
    unsigned d1_ = 6; 
    unsigned d2_ = 6; 
};

/*-----------------------------------------------------------*//**

@class CrapsBet

@brief A bet on the craps table.

Store info that represents a craps bet.

*/

} // namespace App

//----------------------------------------------------------------
