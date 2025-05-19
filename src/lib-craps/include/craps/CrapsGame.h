//----------------------------------------------------------------
//
// File: CrapsGame.h
//
//----------------------------------------------------------------

#pragma once
#include <string>

namespace App {

class CrapsTable;     // fwd
class EventManager;   // fwd
class PlayerManager;  // fwd
    
class CrapsGame
{
public:
    /// @name Lifecycle
    /// @{
    CrapsGame(int argc, char* argv[]);
   ~CrapsGame() = default;
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}

    static const std::string appNameScreen;
    static const std::string appNameExec;

private:
    CrapsTable* initCrapsTable();
    EventManager* initEventManager();
    PlayerManager* initPlayerManager();
};

/*-----------------------------------------------------------*//**

@class CrapsGame

@brief Craps Game Top Level class.

Craps Game.

*/

} // namespace App

//----------------------------------------------------------------
