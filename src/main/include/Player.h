//----------------------------------------------------------------
//
// File: Player.h
//
//----------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <memory>

namespace App {

class CrapsBet;  // fwd

class Player
{
public:
    Player(const std::string& uuid_,
           const std::string& name_,
           unsigned startingBalance = 1000);
    std::string serialize() const;
    static Player deserialize(const std::string& line);

private:    
    std::string uuid;
    std::string name;
    unsigned balance;
    std::vector<std::shared_ptr<class CrapsBet>> bets;
};

} // namespace App

//----------------------------------------------------------------

#if 0

std::string newPlayerId = Gen::generateUuid();
Player p(newPlayerId, "Alice");

#endif
