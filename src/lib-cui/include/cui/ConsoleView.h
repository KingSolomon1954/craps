//----------------------------------------------------------------
//
// File: ConsoleView.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/ViewIntfc.h>

namespace Cui {

class ConsoleView : public Ctrl::ViewIntfc
{
public:
    /// @name Lifecycle
    /// @{
    void run() override;
    /// @}

    /// @name Modifiers
    /// @{
    void displayMessage(const std::string& msg)              override;
    void displayAboutCraps()                                 override;
    Craps::TableId               promptUserToSelectTable()   override;
    std::vector<Craps::PlayerId> promptUserToSelectPlayers() override;
    /// @}

    /// @name Observers
    /// @{
    /// @}

private:
};

/*-----------------------------------------------------------*//**

@class ConsoleView

@brief Console interaction with the user.

more ...

*/

} // namespace Cui

//----------------------------------------------------------------
