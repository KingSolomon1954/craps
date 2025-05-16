//----------------------------------------------------------------
//
// File: Events.h
//
//----------------------------------------------------------------

#pragma once
#include "gen/Uuid.h"

// namespace Gen {
//     class Uuid; // fwd
// }

namespace App {
    
// Events
struct BettingOpened {};
struct BettingClosed {};
struct DiceThrowStart {};
struct DiceThrowEnd { unsigned val; unsigned d1; unsigned d2; };
struct ResolveBetsStart {};
struct ResolveBetsEnd {};
struct PointEstablished { unsigned point; };
struct SevenOut {};
struct NewShooter { const Gen::Uuid& shooterId; };
struct PlayerJoinedTable { const Gen::Uuid& playerId; };
struct PlayerLeftTable { const Gen::Uuid& playerId; };

} // namespace App

//----------------------------------------------------------------
