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

}

//----------------------------------------------------------------
