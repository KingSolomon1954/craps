//----------------------------------------------------------------
//
// File: Player.h
//
//----------------------------------------------------------------

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <gen/Uuid.h>

namespace App {

class CrapsBet;  // fwd

class Player
{
public:
    Player(const std::string& name,
           unsigned startingBalance);
    Player(const Gen::Uuid&,
           const std::string& name,
           unsigned startingBalance);
    std::string serialize() const;
    static Player deserialize(const std::string& line);

private:    
    const Gen::Uuid uuid_;
    const std::string name_;
    unsigned balance_;
    std::vector<std::shared_ptr<class CrapsBet>> bets;
};

} // namespace App

//----------------------------------------------------------------

#if 0

std::string newPlayerId = Gen::generateUuid();
Player p(newPlayerId, "Alice");

#endif
