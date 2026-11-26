//----------------------------------------------------------------
//
// File: AutoOddsOperations.cpp
//
//----------------------------------------------------------------

#include <cui/operations/AutoOddsOperations.h>
#include <cui/core/CuiSender.h>
#include <cui/managers/CarrierBet.h>
#include <cui/managers/SurfaceManager.h>
#include <controller/AutoOdds.h>
#include <controller/CrapsReaders.h>

using namespace Cui;

namespace {

const char*
typeWords(BetName b)
{
    switch (b)
    {
    case BetName::PassLine: return "Pass Line";
    case BetName::DontPass: return "Don't Pass";
    case BetName::Come:     return "Come";
    case BetName::DontCome: return "Don't Come";
    default:                return "?";
    }
}

} // namespace

//----------------------------------------------------------------
//
// What both share
//

void
AutoOddsOperation::send(Ctrl::ReqEditAutoOdds request)
{
    request.playerId = Ctrl::CrapsReaders::getUserPlayerId();
    awaitResponse(CuiSender::instance().reqEditAutoOdds(std::move(request)));
}

//----------------------------------------------------------------

void
AutoOddsOperation::onRspEditAutoOdds(const Ctrl::RspEditAutoOdds& rsp)
{
    if (!rsp.accepted)
    {
        showError(rsp.error, "AutoOdds Error", "Unable to Save AutoOdds");
        return;
    }

    finish();
    SurfaceManager::instance().draw();      // The status line may have changed
    showNotice("AutoOdds", rsp.message);
}

//----------------------------------------------------------------

void
AutoOddsOperation::onErrorAcknowledged()
{
    finish();       // Back to where the person was, still on the stack
}

//----------------------------------------------------------------
//
// Configure AutoOdds
//

void
ConfigureAutoOddsOperation::start()
{
    pickAction();
}

//----------------------------------------------------------------

void
ConfigureAutoOddsOperation::pickAction()
{
    const Ctrl::AutoOdds ao = Ctrl::CrapsReaders::readAutoOdds();
    turnOn_ = !ao.enabled();

    std::vector<DialogPicker::Item> items;
    auto add = [&](std::size_t tag, const std::string& text, const char* desc)
    {
        DialogPicker::Item item;
        item.tag         = tag;
        item.text        = text;
        item.description = desc;
        items.push_back(std::move(item));
    };
    add(0, turnOn_ ? "Turn on" : "Turn off",
        turnOn_ ? "AutoOdds is off now" : "AutoOdds is on now");
    add(1, "List Bets",        "what is set for each bet type");
    add(2, "Set AutoOdds Bet", "pick a bet type and point");
    add(3, "3-4-5 preset",     "3x on 4 and 10, 4x on 5 and 9, 5x on 6 and 8");
    add(4, "Clear all",        "remove every bet");

    step_ = Step::PickAction;
    pickFrom("AutoOdds - Configure", std::move(items));
}

//----------------------------------------------------------------

void
ConfigureAutoOddsOperation::pickType()
{
    const Ctrl::AutoOdds ao = Ctrl::CrapsReaders::readAutoOdds();

    std::vector<DialogPicker::Item> items;
    std::size_t tag = 0;
    for (BetName b : Ctrl::AutoOdds::oddsBetNames())
    {
        DialogPicker::Item item;
        item.tag         = tag++;
        item.text        = typeWords(b);
        item.description = ao.summary(b);
        items.push_back(std::move(item));
    }

    step_ = Step::PickType;
    pickFrom("AutoOdds - Which bet?", std::move(items));
}

//----------------------------------------------------------------

void
ConfigureAutoOddsOperation::pickPoint()
{
    const Ctrl::AutoOdds ao = Ctrl::CrapsReaders::readAutoOdds();

    std::vector<DialogPicker::Item> items;
    std::size_t tag = 0;
    for (unsigned point : Ctrl::AutoOdds::points())
    {
        DialogPicker::Item item;
        item.tag         = tag++;
        item.text        = "Point " + std::to_string(point);
        item.description = ao.setting(betName_, point).toString();
        items.push_back(std::move(item));
    }
    DialogPicker::Item all;
    all.tag         = tag;
    all.text        = "All points";
    all.description = "the same setting for every point";
    items.push_back(std::move(all));

    step_ = Step::PickPoint;
    pickFrom(std::string("AutoOdds - ") + typeWords(betName_) +
                 ", which point?",
             std::move(items));
}

//----------------------------------------------------------------

void
ConfigureAutoOddsOperation::askMultiple()
{
    auto validator = [](const std::string& raw) -> std::string
    {
        Ctrl::AutoOdds::Setting setting;
        return Ctrl::AutoOdds::parseSetting(raw, setting);
    };

    const Ctrl::AutoOdds ao = Ctrl::CrapsReaders::readAutoOdds();
    const auto current = (point_ == 0) ? Ctrl::AutoOdds::Setting{}
                       : ao.setting(betName_, point_);

    step_ = Step::AskMultiple;
    askText(std::string("AutoOdds ") + typeWords(betName_) +
                (point_ == 0 ? ", all points" : " " + std::to_string(point_)),
            "Amount like 50, or 3x the bet (0 clears): ",
            current.isSet() ? current.toString() : std::string(),
            10, std::move(validator));
}

//----------------------------------------------------------------

void
ConfigureAutoOddsOperation::onPick(bool accepted, const Craps::PlayerId&,
                                   Craps::BetId, std::size_t tag)
{
    if (!accepted)
    {
        finish();       // Cancelled; stay on the Configure menu
        return;
    }

    switch (step_)
    {
    case Step::PickAction: onActionPicked(tag); break;
    case Step::PickType:   onTypePicked(tag);   break;
    case Step::PickPoint:  onPointPicked(tag);  break;
    default:               finish();            break;
    }
}

//----------------------------------------------------------------

void
ConfigureAutoOddsOperation::onActionPicked(std::size_t tag)
{
    Ctrl::ReqEditAutoOdds req;

    switch (tag)
    {
    case 0:
        req.action  = Ctrl::ReqEditAutoOdds::Action::SetEnabled;
        req.enabled = turnOn_;
        send(std::move(req));
        break;
    case 1:
        showBetList();
        break;
    case 2:
        pickType();
        break;
    case 3:
        req.action = Ctrl::ReqEditAutoOdds::Action::Preset345;
        send(std::move(req));
        break;
    case 4:
        step_ = Step::ConfirmClear;
        askConfirmation("Clear every AutoOdds multiple?");
        break;
    default:
        finish();
        break;
    }
}

//----------------------------------------------------------------
//
// What is set for each bet type. A notice only; nothing is sent.
//
void
ConfigureAutoOddsOperation::showBetList()
{
    const Ctrl::AutoOdds ao = Ctrl::CrapsReaders::readAutoOdds();
    std::string detail;
    for (BetName b : Ctrl::AutoOdds::oddsBetNames())
    {
        std::string label = std::string(typeWords(b)) + ":";
        label.resize(12, ' ');
        detail += label + ao.summary(b) + "\n";
    }
    detail += "\nAutoOdds is ";
    detail += ao.enabled() ? "on." : "off.";
    finish();
    showNotice("AutoOdds - List Bets", detail);
}

//----------------------------------------------------------------

void
ConfigureAutoOddsOperation::onTypePicked(std::size_t tag)
{
    const auto& names = Ctrl::AutoOdds::oddsBetNames();
    if (tag >= names.size())
    {
        finish();
        return;
    }
    betName_ = names[tag];
    pickPoint();
}

//----------------------------------------------------------------

void
ConfigureAutoOddsOperation::onPointPicked(std::size_t tag)
{
    const auto& points = Ctrl::AutoOdds::points();
    if (tag > points.size())
    {
        finish();
        return;
    }
    point_ = (tag == points.size()) ? 0u : points[tag];    // One past: all
    askMultiple();
}

//----------------------------------------------------------------

void
ConfigureAutoOddsOperation::onText(bool accepted, const std::string& text)
{
    if (!accepted)
    {
        finish();
        return;
    }

    Ctrl::ReqEditAutoOdds req;
    req.betName  = betName_;
    req.point    = point_;
    Ctrl::AutoOdds::Setting setting;
    Ctrl::AutoOdds::parseSetting(text, setting);    // Validated already
    req.multiple = setting.multiple;
    req.amount   = setting.amount;
    req.action   = Ctrl::ReqEditAutoOdds::Action::Set;
    send(std::move(req));
}

//----------------------------------------------------------------

void
ConfigureAutoOddsOperation::onConfirm(SurfaceBase::OperationResult result)
{
    if (result != SurfaceBase::OperationResult::Yes)
    {
        finish();
        return;
    }

    Ctrl::ReqEditAutoOdds req;
    req.action = Ctrl::ReqEditAutoOdds::Action::Clear;
    send(std::move(req));
}

//----------------------------------------------------------------
//
// Set or unset AutoOdds from the bet amount dialog
//

void
SetAutoOddsOperation::start()
{
    const auto& cb = CarrierBet::instance();
    if (!cb.isOddsBet() || !Ctrl::AutoOdds::isOddsBetName(cb.getBetName()))
    {
        finish();
        return;
    }

    // The point the odds are for. A bet that has none yet (a line bet
    // being made) goes by the table's point.
    unsigned point = cb.getPivot();
    if (point == 0)
    {
        Craps::TableId tableId;
        Gen::ErrorPass ep;
        if (Ctrl::CrapsReaders::getActiveCrapsTable(tableId, ep) ==
                Gen::ReturnCode::Success)
        {
            Ctrl::CrapsReaders::readTablePoint(tableId, point, ep);
        }
    }
    if (!Ctrl::AutoOdds::isPoint(point))
    {
        showNotice("AutoOdds", "There is no point yet, so there is no "
                   "point to set AutoOdds for.");
        finish();
        return;
    }

    Ctrl::ReqEditAutoOdds req;
    req.action  = Ctrl::ReqEditAutoOdds::Action::Set;
    req.betName = cb.getBetName();
    req.point   = point;
    req.amount  = cb.getOddsAmount();       // Zero unsets
    send(std::move(req));
}

//----------------------------------------------------------------
