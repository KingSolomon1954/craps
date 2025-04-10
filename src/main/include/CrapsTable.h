//----------------------------------------------------------------
//
// File: CrapsTable.h
//
//----------------------------------------------------------------

#pragma once

#include "Globals.h"
#include "CrapsBet.h"
#include "Dice.h"

namespace App {

class CrapsTable
{
public:
    /// @name Lifecycle
    /// @{
    CrapsTable();
   ~CrapsTable() = default;
    /// @}

    /// @name Modifiers
    /// @{
    /// @}

    /// @name Observers
    /// @{
    /// @}
    
private:
    Dice dice;
};

/*-----------------------------------------------------------*//**

@class CrapsTable

@brief Craps Table.

Responsible for operating a Craps Table.

*/

} // namespace App

//----------------------------------------------------------------
