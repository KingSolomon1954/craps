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
    void setGameController(Ctrl::GameControllerIntfc* pCtrlIntfc) override;
    void run() override;
    /// @}

    /// @name Modifiers
    /// @{
    void displayMessage(const std::string& msg) override;
    void displayAboutCraps() override;
    Ctrl::TableManager::TableId promptUserToSelectTable(
        const Ctrl::TableManager::TableDescriptions& tds) override;
    std::vector<Ctrl::PlayerManager::PlayerId> promptUserToSelectPlayers(
        const Ctrl::PlayerManager::PlayerDescriptions& pds) override;
    /// @}

    /// @name Observers
    /// @{
    /// @}

private:
    Ctrl::GameControllerIntfc* pCtrlIntfc_ = nullptr;
};

/*-----------------------------------------------------------*//**

@class ConsoleView

@brief Console interaction with the user.

more ...

*/

} // namespace Cui

//----------------------------------------------------------------
