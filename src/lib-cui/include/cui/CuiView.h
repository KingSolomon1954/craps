//----------------------------------------------------------------
//
// File: CuiView.h
//
//----------------------------------------------------------------

#pragma once

#include <controller/ViewInterface.h>

namespace Cui {

class CuiView : public Ctrl::ViewInterface
{
public:
    /// @name Lifecycle
    /// @{
    static CuiView& instance();
   ~CuiView() = default;
    /// @}

    /// @name Show-ers
    /// @{
    void showSuccess(
        Ctrl::EventType    orgEventType,
        uint64_t           correlationId) override;
    void showErrorDialog(
        Ctrl::EventType    orgEventType,
        uint64_t           correlationId,
        const std::string& diag) override;
    void showMakeBetSuccess(
        Craps::BetId betId,
        uint64_t     correlationId) override;
    void showMakeOddsBetSuccess(
        Craps::BetId betId,
        uint64_t     correlationId) override;
    void showMakeBetAutoSuccess(
        Craps::BetId betId,
        uint64_t     correlationId) override;
    void showMakeBetAutoError(
        Ctrl::EventType        orgEventType,
        uint64_t               correlationId,
        const Craps::PlayerId& playerId,
        const std::string&     diag) override;
    void showRollDiceCountDown(
        uint64_t               correlationId,
        int                    numSeconds) override;
    void showRollDiceAnimation(
        uint64_t               correlationId) override;
    void showProgramExit() override;
    /// @}

private:
    CuiView() = default;
};

/*-----------------------------------------------------------*//**

@class CuiView

@brief implements the controller ViewInterface

Responsibilities of ConsoleView:

@li Implements the UI ViewInterface
@li Is the adapter between the Controller's view vocabulary and the
    CUI's surface vocabulary.
@li Dispatches events (no console-side logic). Just forwards them to
    Controller::ViewCommands.
*/

} // namespace Cui

//----------------------------------------------------------------
