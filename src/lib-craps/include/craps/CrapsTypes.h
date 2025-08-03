//----------------------------------------------------------------
//
// File: CrapsTypes.h
//
//----------------------------------------------------------------

#pragma once

#include <string>
#include <memory>
#include <gen/Uuid.h>

namespace Craps {

class Player;    // fwd
class CrapsBet;  // fwd
    
using TableId   = std::string;
using PlayerId  = Gen::Uuid;
using PlayerPtr = std::shared_ptr<class Player>;
using BetId     = unsigned;
using BetPtr    = std::shared_ptr<class CrapsBet>;
    
}

//----------------------------------------------------------------
