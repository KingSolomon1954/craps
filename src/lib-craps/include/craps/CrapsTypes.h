//----------------------------------------------------------------
//
// File: CrapsTypes.h
//
//----------------------------------------------------------------

#pragma once

#include <string>
#include <memory>
#include <deque>
#include <vector>
#include <unordered_set>
#include <gen/Uuid.h>

namespace Craps {

class Player;    // fwd
class CrapsBet;  // fwd
class Dice;      // fwd
    
using TableId   = std::string;
    
using PlayerId  = Gen::Uuid;
using PlayerPtr = std::shared_ptr<class Player>;
using PlayerIds = std::vector<Craps::PlayerId>;
    
using BetId     = unsigned;
using BetPtr    = std::shared_ptr<class CrapsBet>;
using BetIds    = std::vector<Craps::BetId>;
    
using RecentRolls = std::deque<Craps::Dice>;

//----------------------------------------------------------------
// Dice-total groupings.  Use "if (PointNums.contains(value))" in
// place of hand-written chains of comparisons.
//----------------------------------------------------------------

using DiceNums = std::unordered_set<unsigned>;

inline const DiceNums PointNums        = {4, 5, 6, 8, 9, 10};
inline const DiceNums FieldNums        = {2, 3, 4, 9, 10, 11, 12};
inline const DiceNums FieldLoseNums    = {5, 6, 7, 8};
inline const DiceNums CrapsNums        = {2, 3, 12};
inline const DiceNums CrapsNumsBar12   = {2, 3};         // craps that win Don't Pass
inline const DiceNums NaturalNums      = {7, 11};
inline const DiceNums BookEnds         = {2, 3, 11, 12}; // Horn, C&E
inline const DiceNums WorldNums        = {2, 3, 7, 11, 12};
inline const DiceNums HardwayNums      = {4, 6, 8, 10};
inline const DiceNums AceTwelveNums    = {2, 12};        // Horn pays 30:1
inline const DiceNums ThreeElevenNums  = {3, 11};        // Horn pays 15:1
inline const DiceNums FourTenNums      = {4, 10};
inline const DiceNums FiveNineNums     = {5, 9};
inline const DiceNums SixEightNums     = {6, 8};

}

//----------------------------------------------------------------
