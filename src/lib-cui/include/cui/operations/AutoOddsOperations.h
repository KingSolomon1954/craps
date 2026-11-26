//----------------------------------------------------------------
//
// File: AutoOddsOperations.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/operations/Operation.h>
#include <controller/GameEvents.h>
#include <craps/EnumBetName.h>

namespace Cui {

/// What the two AutoOdds operations share: one ReqEditAutoOdds, and a
/// notice with the answer. An error leaves the person where they were, on
/// the Configure menu or in the bet amount dialog, both still on the stack.
class AutoOddsOperation : public Operation
{
public:
    void onRspEditAutoOdds(const Ctrl::RspEditAutoOdds& rsp) override;
    void onErrorAcknowledged() override;

protected:
    void send(Ctrl::ReqEditAutoOdds request);
};

/// AutoOdds, from the Configuration menu.
///
/// One pick list of things to do:
///
///   turn on / turn off   one request
///   list bets            a notice, no request
///   set a multiple       pick bet type, pick point (or all), type the
///                        multiple (0 clears), one request
///   3-4-5 preset         one request
///   clear all            confirm, one request
///
/// The pick lists and prompts each pop themselves and report here, so
/// step_ says which one just answered.
class ConfigureAutoOddsOperation : public AutoOddsOperation
{
public:
    void start() override;
    void onPick(bool accepted, const Craps::PlayerId& playerId,
                Craps::BetId betId, std::size_t tag) override;
    void onText(bool accepted, const std::string& text) override;
    void onConfirm(SurfaceBase::OperationResult result) override;

private:
    enum class Step { PickAction, PickType, PickPoint, AskMultiple,
                      ConfirmClear };

    // Each step: show a list or a prompt
    void pickAction();
    void pickType();
    void pickPoint();
    void askMultiple();

    // Each step: what the answer means
    void onActionPicked(std::size_t tag);
    void onTypePicked(std::size_t tag);
    void onPointPicked(std::size_t tag);

    void showBetList();

    Step     step_     = Step::PickAction;
    bool     turnOn_   = false;              // What the first line offers
    BetName  betName_  = BetName::Invalid;   // The bet type picked
    unsigned point_    = 0;                  // The point picked; 0 means all
};

/// From the bet amount dialog while entering odds: set AutoOdds for this
/// bet type and point to exactly the amount entered. An amount of zero
/// unsets it. One request, and the player stays in the amount dialog, with
/// a notice.
class SetAutoOddsOperation : public AutoOddsOperation
{
public:
    void start() override;
};

} // namespace Cui

//----------------------------------------------------------------
