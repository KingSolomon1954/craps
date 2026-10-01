//----------------------------------------------------------------
//
// File: WorkOrderDispatcher.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/WorkOrder.h>

//----------------------------------------------------------------

namespace Cui {

class WorkOrderDispatcher
{
public:
    WorkOrderDispatcher() = default;
   ~WorkOrderDispatcher() = default;

    WorkOrderDispatcher(const WorkOrderDispatcher&)            = delete;
    WorkOrderDispatcher& operator=(const WorkOrderDispatcher&) = delete;
    WorkOrderDispatcher(WorkOrderDispatcher&&)                 = delete;
    WorkOrderDispatcher& operator=(WorkOrderDispatcher&&)      = delete;
    
    void dispatch(const WorkOrder& wo);  // Main entry point
    
private:
    void process(const WorkOrderKey& wo);
    void process(const WorkOrderSurface& wo);
    void process(const WorkOrderEvent& wo);
    
    void process(const Ctrl::UslBettingClosed& ev);
    void process(const Ctrl::UslDiceThrowStart& ev);
    void process(const Ctrl::UslDiceNewValue& ev);
    void process(const Ctrl::UslResolveBetsEnd& ev);
    void process(const Ctrl::UslPointEstablished& ev);
    void process(const Ctrl::UslSevenOut& ev);
    void process(const Ctrl::UslPassLineWinner& ev);
    void process(const Ctrl::UslNewShooter& ev);
    void process(const Ctrl::UslBettingOpened& ev);
    void process(const Ctrl::UslBetMade& ev);
    void process(const Ctrl::UslBetResolved& ev);
    void process(const Ctrl::UslTableBalanceChanged& ev);
    void process(const Ctrl::UslPlayerBalanceChanged& ev);
};

/*-----------------------------------------------------------*//**

@class WorkOrderDispatcher

@brief Dispatches work orders to the many CUI presentation classes

Receives all WorkOrders coming off the CuiThread. Translates work orders
and GameEvents into primitive data types, then calls into various
windows using hard coded knowledge of which window wants what
information.
*/

} // namespace Cui

//----------------------------------------------------------------
