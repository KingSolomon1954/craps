//----------------------------------------------------------------
//
// File: QuickBetsOperations.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/operations/Operation.h>
#include <controller/BetSpec.h>
#include <string>
#include <vector>

namespace Cui {

/// Pick one of the user's QuickBets entries and apply it. The pick list
/// shows each entry's name and a short description of its bets. Applying
/// is one request; the answer says what was placed and what was skipped.
class ApplyQuickBetsOperation : public Operation
{
public:
    void start() override;
    void onPick(bool accepted, const Craps::PlayerId& playerId,
                Craps::BetId betId, std::size_t tag) override;
    void onRspApplyQuickBets(const Ctrl::RspApplyQuickBets& rsp) override;

private:
    std::vector<std::string> names_;    // What the list offered, by tag
};

/// What the two operations that change QuickBets entries share: they send
/// a ReqEditQuickBets, and the answer is a notice on the screen the person
/// came from. An error also leaves the person where they were, because the
/// bet amount dialog or the Configure menu is still on the stack.
class EditQuickBetsOperation : public Operation
{
public:
    void onRspEditQuickBets(const Ctrl::RspEditQuickBets& rsp) override;
    void onErrorAcknowledged() override;

protected:
    void send(Ctrl::ReqEditQuickBets request);

    /// Title of the notice after a successful change
    virtual const char* doneTitle() const = 0;
};

/// Save the bet being entered into a QuickBets entry. DialogBetAmount
/// starts this with the amount already in CarrierBet. The player picks a
/// new entry or an existing one to add to. A new entry asks for a name.
/// Afterwards the player is back at DialogBetAmount, with a notice saying
/// what happened, so the bet can still be placed.
class SaveToQuickBetsOperation : public EditQuickBetsOperation
{
public:
    void start() override;
    void onQuickBetsChoice(bool accepted, std::size_t index) override;
    void onText(bool accepted, const std::string& text) override;

private:
    const char* doneTitle() const override { return "Saved to QuickBets"; }
    void askForName();
    void send(Ctrl::ReqEditQuickBets::Action action, const std::string& name);

    Ctrl::BetSpec            spec_;     // The bet being saved
    std::vector<std::string> names_;    // The entries the dialog offered
};

/// Managing QuickBets, from the Configuration menu.
///
/// Pick an entry, then pick what to do with it: rename it, change the
/// amount of one of its bets, remove one of its bets, or delete it. Each
/// action asks what it needs (a name, a bet, an amount, a confirmation)
/// and then sends one ReqEditQuickBets. New entries are made from the bet
/// amount dialog, where the bet is already chosen.
///
/// The pick lists and prompts each pop themselves and report here, so
/// step_ says which one just answered.
class ManageQuickBetsOperation : public EditQuickBetsOperation
{
public:
    void start() override;
    void onPick(bool accepted, const Craps::PlayerId& playerId,
                Craps::BetId betId, std::size_t tag) override;
    void onText(bool accepted, const std::string& text) override;
    void onConfirm(SurfaceBase::OperationResult result) override;

private:
    enum class Step { PickEntry, PickAction, PickBet, AskName, AskAmount,
                      ConfirmDelete };
    using Action = Ctrl::ReqEditQuickBets::Action;

    const char* doneTitle() const override { return "QuickBets changed"; }

    // Each step: show a list or a prompt
    void pickAction();
    void pickBet(Action action);
    void askName();
    void askAmount();

    // Each step: what the answer means
    void onEntryPicked(std::size_t tag);
    void onActionPicked(std::size_t tag);
    void onBetPicked(std::size_t tag);

    void removeABet();
    void sendRequest(const std::string& text = std::string());

    Step                     step_ = Step::PickEntry;
    Action                   action_ = Action::Rename;
    std::vector<std::string> names_;       // The entries the list offered
    std::string              entry_;       // The entry being managed
    std::size_t              betIndex_ = 0;
};

} // namespace Cui

//----------------------------------------------------------------
