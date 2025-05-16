//----------------------------------------------------------------
//
// File: Player.h
//
//----------------------------------------------------------------

#pragma once

#include <string>
#include <memory>
#include <list>
#include <nlohmann/json.hpp>
#include <gen/Uuid.h>
#include "Bank.h"
#include "CrapsTable.h"
#include "CrapsBet.h"
#include "CrapsBetIntfc.h"

using json = nlohmann::json;

namespace App {

class CrapsBet;  // fwd
class DecisionRecort;  // fwd

class Player
{
public:
    /// @name Lifecycle
    /// @{
    Player();
    Player(const std::string& name,
           unsigned startingBalance);
    Player(const Gen::Uuid&,
           const std::string& name,
           unsigned startingBalance);
    
    bool saveToFile(const std::string& path) const;
    bool loadFromFile(const std::string& path);
    
//    static Player deserialize(const std::string& line);
//    std::string serialize() const;
    /// @}

    /// @name Modifiers
    /// @{
    void processWin (const DecisionRecord& dr);
    void processLose(const DecisionRecord& dr);
    void processKeep(const DecisionRecord& dr);
    void fromJson(const json& j);
    /// @}
    
    /// @name Observers
    /// @{
    const Gen::Uuid& getUuid() const;
    const std::string& getName() const;
    json toJson() const;
    /// @}

private:    
    Gen::Uuid uuid_;
    std::string name_;
    Bank wallet_;
    using BetIntfcPtr = std::shared_ptr<class CrapsBetIntfc>;
    std::list<BetIntfcPtr> bets_;

    bool removeBetByPtr(BetIntfcPtr& pBet);
    BetIntfcPtr findBetById(unsigned betId) const;
    void diagBadBetId(const std::string& funcName, unsigned betId) const;
};

/*-----------------------------------------------------------*//**

@class Player

@brief Craps player

Player Responsibilities:

@li maintains one file per player

@li all player files in the players directory

@li Know how to read/write its own data

@li Export a to_json() and from_json() method

*/
    
} // namespace App

//----------------------------------------------------------------
