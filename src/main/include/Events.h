//----------------------------------------------------------------
//
// File: Events.h
//
//----------------------------------------------------------------

#pragma once

namespace App {

// Events
struct BettingOpened {};
struct BettingClosed {};
struct DiceThrowStart {};
struct DiceThrowEnd { unsigned val; unsigned d1; unsigned d2;};
struct ResolveBetsStart {};
struct ResolveBetsEnd {};
struct PointEstablished { unsigned point; };
struct SevenOut {};
struct NewShooter {};
struct PlayerJoinedTable {};
struct PlayerLeftTable {};

} // namespace App

//----------------------------------------------------------------
