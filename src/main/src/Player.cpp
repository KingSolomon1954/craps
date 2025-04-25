//----------------------------------------------------------------
//
// File: Player.cpp
//
//----------------------------------------------------------------

#include "Player.h"
#include <sstream>
#include <fstream>

using namespace App;

//----------------------------------------------------------------
//
// Constructor.
// 
Player::Player(
    const std::string& name,
    unsigned startingBalance)
    : uuid_(Gen::generateUuid())
    , name_(name)
    , balance_(startingBalance)
{
    // empty
}

//----------------------------------------------------------------
//
// Constructor.
// 
Player::Player(
    const Gen::Uuid& uuid,
    const std::string& name,
    unsigned startingBalance)
    : uuid_(uuid)
    , name_(name)
    , balance_(startingBalance)
{
    // empty
}

//----------------------------------------------------------------
//
// For saving/loading
//
std::string
Player::serialize() const
{
    return uuid_ + "," + name_ + "," + std::to_string(balance_);
}

//----------------------------------------------------------------

Player
Player::deserialize(const std::string& line)
{
    std::istringstream ss(line);
    std::string uuidStr, nameStr, balanceStr;
    std::getline(ss, uuidStr, ',');
    std::getline(ss, nameStr, ',');
    std::getline(ss, balanceStr);
    return Player(uuidStr, nameStr, std::stol(balanceStr));
}

//----------------------------------------------------------------
