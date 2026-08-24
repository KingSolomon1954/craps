//----------------------------------------------------------------
//
// File: CrapsUi.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/ConsoleManager.h>

namespace Cui
{
    
class CrapsUI
{
public:
    CrapsUI() = default;
   ~CrapsUI() = default;
   
    void init();
    void run();
    
private:
    void createSurfaces();
};


/*-----------------------------------------------------------*//**

@class CrapsUi

@brief Specialized surface type for full screens

@li Provides the generic UI surface contract for full screens.

*/

} // namespace Cui

//----------------------------------------------------------------

