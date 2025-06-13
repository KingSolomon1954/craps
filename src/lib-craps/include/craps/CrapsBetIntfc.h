//----------------------------------------------------------------
//
// File: CrapsBetInfc.h
//
//----------------------------------------------------------------

#pragma once

#include <gen/Uuid.h>
#include <gen/Timepoint.h>
#include <craps/EnumBetName.h>

namespace Craps {

class CrapsBetIntfc
{
public:
    /// @name Lifecycle
    /// @{
    /// @}

    /// @name Modifiers
    /// @{
    virtual void setOffComeOutRoll() = 0;
    virtual void setOnComeOutRoll() = 0;
    virtual void setHardwayOff() = 0;
    virtual void setHardwayOn() = 0;
    /// @}

    /// @name Observers
    /// @{
    virtual const Gen::Uuid& playerId()  const = 0;
    virtual unsigned betId()             const = 0;
    virtual BetName betName()            const = 0;
    virtual unsigned pivot()             const = 0;
    virtual unsigned contractAmount()    const = 0;
    virtual unsigned oddsAmount()        const = 0;
    virtual bool offComeOutRoll()        const = 0;
    virtual bool hardwayWorking()        const = 0;
    virtual unsigned distance()          const = 0;
    virtual Gen::Timepoint whenCreated() const = 0;
    virtual Gen::Timepoint whenDecided() const = 0;
    /// @}
};

/*-----------------------------------------------------------*//**

@class CrapsBetIntfc Interface for Craps Bats

@brief A bet on the craps table.

An interface to manage a craps bet on the table.

*/

} // namespace Craps

//----------------------------------------------------------------
