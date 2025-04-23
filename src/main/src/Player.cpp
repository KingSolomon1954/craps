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

Player::Player(
    const std::string& uuid_,
    const std::string& name_,
    unsigned startingBalance)
    : uuid(uuid_)
    , name(name_)
    , balance(startingBalance)
{
}

//----------------------------------------------------------------
//
// For saving/loading
//
std::string
Player::serialize() const
{
    return uuid + "," + name + "," + std::to_string(balance);
}

//----------------------------------------------------------------

Player
Player::deserialize(const std::string& line)
{
    std::istringstream ss(line);
    std::string id, name, balanceStr;
    std::getline(ss, id, ',');
    std::getline(ss, name, ',');
    std::getline(ss, balanceStr);
    return Player(id, name, std::stod(balanceStr));
}

//----------------------------------------------------------------
