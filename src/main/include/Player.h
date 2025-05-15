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
#include "Bank.h"
#include "CrapsTable.h"
#include "CrapsBet.h"
#include "CrapsBetIntfc.h"

namespace App {

class CrapsBet;  // fwd
class DecisionRecort;  // fwd

class Player
{
public:
    /// @name Lifecycle
    /// @{
    Player(const std::string& name,
           unsigned startingBalance);
    Player(const Gen::Uuid&,
           const std::string& name,
           unsigned startingBalance);
    /// @}

    /// @name Modifiers
    /// @{
    void processWin (const DecisionRecord& dr);
    void processLose(const DecisionRecord& dr);
    void processKeep(const DecisionRecord& dr);
    static Player deserialize(const std::string& line);
    /// @}
    
    /// @name Observers
    /// @{
    const Gen::Uuid& getUuid() const;
    std::string serialize() const;
    /// @}

private:    
    const Gen::Uuid uuid_;
    const std::string name_;
    Bank wallet_;
    using BetIntfcPtr = std::shared_ptr<class CrapsBetIntfc>;
    std::vector<BetIntfcPtr> bets_;

    bool removeBetByPtr(BetIntfcPtr& pBet);
    BetIntfcPtr findBetById(unsigned betId) const;
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
