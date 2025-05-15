//----------------------------------------------------------------
//
// File: PlayerManager.cpp
//
//----------------------------------------------------------------

#include "PlayerManager.h"
#include "DecisionRecord.h"

using namespace App;

/*-----------------------------------------------------------*//**

Constructor

*/
PlayerManager::PlayerManager()
{
    // Empty.
}

/*-----------------------------------------------------------*//**

Disburse winning decision record to player.

*/
void
PlayerManager::disburseWin(const DecisionRecord& dr) const
{
    (void) dr;
    // TODO
    // Lookup playerId
    // Call playerIntfc->playerWin(dr)
}

/*-----------------------------------------------------------*//**

Disburse losing decision record to player.

*/
void
PlayerManager::disburseLose(const DecisionRecord& dr) const
{
    (void) dr;
    // TODO
    // Lookup playerId
    // Call playerIntfc->playerLose(dr)
}

/*-----------------------------------------------------------*//**

Disburse keep decision record to player.

*/
void
PlayerManager::disburseKeep(const DecisionRecord& dr) const
{
    (void) dr;
    // TODO
    // Lookup playerId
    // Call playerIntfc->playerKeep(dr)
}

//----------------------------------------------------------------
