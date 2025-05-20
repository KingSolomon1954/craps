//----------------------------------------------------------------
//
// File: Dice.h
//
//----------------------------------------------------------------

#pragma once

namespace Craps {

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
    void set(unsigned d1, unsigned d2);  // Primarily for testing
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

@class Dice

@brief Dice

Two die.

*/

} // namespace Craps

//----------------------------------------------------------------
