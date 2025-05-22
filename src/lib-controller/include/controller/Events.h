//----------------------------------------------------------------
//
// File: Events.h
//
//----------------------------------------------------------------

#pragma once
#include <gen/Uuid.h>

namespace Ctrl {
    
// Events
struct BettingOpened {};
struct BettingClosed {};
struct DiceThrowStart {};
struct DiceThrowEnd {};
struct AnnounceDiceNumber {unsigned val; unsigned d1; unsigned d2;};
struct ResolveBetsStart {};
struct ResolveBetsEnd {};
struct PointEstablished {unsigned point;};
struct SevenOut {};
struct PassLineWinner {};
struct NewShooter        {const Gen::Uuid& shooterId;};
struct PlayerJoinedTable {const Gen::Uuid& playerId;};
struct PlayerLeftTable   {const Gen::Uuid& playerId;};

} // namespace Ctrl

//----------------------------------------------------------------
