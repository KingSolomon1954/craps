//----------------------------------------------------------------
//
// File: PlayerAreaAllPlayers.cpp
//
//----------------------------------------------------------------

#include <cui/panels/PlayerAreaAllPlayers.h>
#include <cui/ColorManager.h>
#include <controller/CrapsReaders.h>
#include <craps/CrapsTypes.h>
#include <gen/ErrorPass.h>
#include <gen/Logger.h>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <string>

using namespace Cui;

//----------------------------------------------------------------

PlayerAreaAllPlayers::PlayerAreaAllPlayers(int height, int width)
    : winHeight_(height)
    , winWidth_(width)
{
}

//----------------------------------------------------------------
//
// Called by WindowPlayerArea after the base WINDOW pWin_ is created
// and after the list of players is known.
//

void
PlayerAreaAllPlayers::init(
    WINDOW* pWin,
    const std::vector<Craps::PlayerId>& playerIds)
{
    pWin_ = pWin;
    buildBetInfo();
    buildPlayerInfo(playerIds);
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::buildBetInfo()
{
    bets_.clear();
    bets_.reserve(16 + 12 + 16);

    auto addBet =
        [this](std::string_view label, int row, int section)
        {
            Bet bet;
            bet.label = label;
            bet.row = row;
            bet.section = section;
            bet.state.fill(BetState::None);
            bets_.push_back(bet);
        };

    int row = Layout::FirstBetRow;

    // Section 0
    addBet("PassLine",   row++, 0);
    addBet("Come",       row++, 0);
    addBet("Come4",      row++, 0);
    addBet("Come5",      row++, 0);
    addBet("Come6",      row++, 0);
    addBet("Come8",      row++, 0);
    addBet("Come9",      row++, 0);
    addBet("Come10",     row++, 0);
    addBet("DontPass",   row++, 0);
    addBet("DontCome",   row++, 0);
    addBet("DontCome4",  row++, 0);
    addBet("DontCome5",  row++, 0);
    addBet("DontCome6",  row++, 0);
    addBet("DontCome8",  row++, 0);
    addBet("DontCome9",  row++, 0);
    addBet("DontCome10", row++, 0);

    row = Layout::FirstBetRow;

    // Section 1
    addBet("Place4", row++, 1);
    addBet("Place5", row++, 1);
    addBet("Place6", row++, 1);
    addBet("Place8", row++, 1);
    addBet("Place9", row++, 1);
    addBet("Place10", row++, 1);
    addBet("Hard4", row++, 1);
    addBet("Hard6", row++, 1);
    addBet("Hard8", row++, 1);
    addBet("Hard10", row++, 1);
    addBet("C&E", row++, 1);
    addBet("Field", row++, 1);

    row = Layout::FirstBetRow;

    // Section 2
    addBet("Buy4", row++, 2);
    addBet("Buy5", row++, 2);
    addBet("Buy6", row++, 2);
    addBet("Buy8", row++, 2);
    addBet("Buy9", row++, 2);
    addBet("Buy10", row++, 2);
    addBet("Lay4", row++, 2);
    addBet("Lay5", row++, 2);
    addBet("Lay6", row++, 2);
    addBet("Lay8", row++, 2);
    addBet("Lay9", row++, 2);
    addBet("Lay10", row++, 2);
    addBet("AnyC", row++, 2);
    addBet("Any7", row++, 2);
    addBet("Horn", row++, 2);
    addBet("World", row++, 2);
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::buildPlayerInfo(
    const std::vector<Craps::PlayerId>& playerIds)
{
    players_.clear();
    players_.reserve(LayoutAllPlayers::MaxPlayers);

    Craps::PlayerId userPlayerId;
    Gen::ErrorPass ep;

    auto rc = Ctrl::CrapsReaders::getUserPlayer(userPlayerId, ep);
    assert(rc == Gen::ReturnCode::Success);

    std::vector<Craps::PlayerId> orderedIds;
    orderedIds.reserve(LayoutAllPlayers::MaxPlayers);

    // User always gets the first column.
    auto userIt = std::find(playerIds.begin(), playerIds.end(),
                            userPlayerId);

    if (userIt == playerIds.end())
    {
        // Either display the supplied order, or leave players_ empty.
        return;
    }

    orderedIds.push_back(userPlayerId);

    // Preserve the existing order for everyone else.
    for (const auto& pid : playerIds)
    {
        if (pid != userPlayerId)
            orderedIds.push_back(pid);

        if (orderedIds.size() == LayoutAllPlayers::MaxPlayers)
            break;
    }

    for (const auto& playerId : orderedIds)
    {
        std::string name;

        rc = Ctrl::CrapsReaders::readPlayerName(playerId, name, ep);
        assert(rc == Gen::ReturnCode::Success);

        players_.push_back(
        {
            .playerId = playerId,
            .initial  = chooseInitial(name)
        });
    }
}

//----------------------------------------------------------------

wchar_t
PlayerAreaAllPlayers::chooseInitial(std::string_view name) const
{
    for (unsigned char ch : name)
    {
        if (std::isalpha(ch))
            return static_cast<wchar_t>(std::toupper(ch));
    }

    return L'?';
}

//----------------------------------------------------------------

short
PlayerAreaAllPlayers::playerColorPair(std::size_t index) const
{
    if (index >= players_.size())
        return 0;

    const auto color =
        ColorManager::instance().getPlayerColor(players_[index].playerId);

    // ColorManager should have an assignment for every player displayed
    // by this window. The dispatcher guarantees that ColorManager processes
    // player join/leave events before windows receive them.
    assert(color.has_value());

    if (!color.has_value())
        return 0;

    return ColorManager::instance().pair(*color);
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::drawInternalBorders()
{
    using L = Layout;

    // Vertical lines
    mvwvline(pWin_, 0, L::col1_2, 0, winHeight_);
    mvwvline(pWin_, 0, L::col2_3, 0, winHeight_);
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::drawStaticContent()
{
    drawPlayerHeaders();
    drawBetLabels();
    drawBetMarkers();
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::drawPlayerHeaders()
{
    using L = Layout;

    for (int section = 0; section < 3; ++section)
    {
        for (int player = 0;
             player < LayoutAllPlayers::MaxPlayers;
             ++player)
        {
            const wchar_t initial =
                player < static_cast<int>(players_.size())
                    ? players_[player].initial
                    : L::DoubleDash;

            const short colorPair =
                player < static_cast<int>(players_.size())
                    ? playerColorPair(static_cast<std::size_t>(player))
                    : 0;

            const int col =
                L::SectionX[section] + L::SectionW[section] +
                player * L::PlayerStride;

            drawWideCharacter(L::HeaderRow, col, initial, colorPair);
        }
    }
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::drawBetLabels()
{
    for (const Bet& bet : bets_)
    {
        const int col = Layout::SectionX[bet.section] + 1;

        mvwaddnstr(pWin_, bet.row, col,
                   bet.label.data(),
                   static_cast<int>(bet.label.size()));
    }
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::drawBetMarker(
    std::size_t betIndex,
    std::size_t playerIndex)
{
    if (pWin_ == nullptr)
        return;

    if (playerIndex >= players_.size())
        return;

    int row = 0;
    int col = 0;

    if (!betPosition(betIndex, playerIndex, row, col))
        return;

    wchar_t marker = Layout::Dot;

    switch (bets_[betIndex].state[playerIndex])
    {
        case BetState::None:
            marker = Layout::Dot;
            break;

        case BetState::Bet:
            marker = Layout::HollowCircle;
            break;

        case BetState::BetWithOdds:
            marker = Layout::FilledCircle;
            break;
    }

    drawWideCharacter(
        row,
        col,
        marker,
        playerColorPair(playerIndex));
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::drawBetMarkers()
{
    for (std::size_t betIndex = 0;
         betIndex < bets_.size();
         ++betIndex)
    {
        for (std::size_t playerIndex = 0;
             playerIndex < LayoutAllPlayers::MaxPlayers;
             ++playerIndex)
        {
            if (playerIndex < players_.size())
            {
                drawBetMarker(betIndex, playerIndex);
            }
            else
            {
                int row = 0;
                int col = 0;

                if (betPosition(betIndex, playerIndex, row, col))
                {
                    drawWideCharacter(
                        row, col, Layout::Dot, 0);
                }
            }
        }
    }
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::drawWideCharacter(
    int row,
    int col,
    wchar_t ch,
    short colorPair)
{
    if (colorPair != 0)
        wattron(pWin_, COLOR_PAIR(colorPair));

    wchar_t text[2] = { ch, L'\0' };

    mvwaddwstr(pWin_, row, col, text);

    if (colorPair != 0)
        wattroff(pWin_, COLOR_PAIR(colorPair));
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::setBetState(
    std::size_t betIndex,
    std::size_t playerIndex,
    BetState state)
{
    if (betIndex >= bets_.size())
        return;

    if (playerIndex >= LayoutAllPlayers::MaxPlayers)
        return;

    bets_[betIndex].state[playerIndex] = state;
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::setBetState(
    const std::string_view& betName,
    std::size_t playerIndex,
    BetState state)
{
    for (std::size_t i = 0; i < bets_.size(); ++i)
    {
        if (bets_[i].label == betName)
        {
            setBetState(i, playerIndex, state);
            return;
        }
    }
}

//----------------------------------------------------------------

bool
PlayerAreaAllPlayers::betPosition(
    std::size_t betIndex,
    std::size_t playerIndex,
    int& row,
    int& col) const
{
    using L = Layout;

    if (betIndex >= bets_.size())
        return false;

    if (playerIndex >= LayoutAllPlayers::MaxPlayers)
        return false;

    const Bet& bet = bets_[betIndex];

    row = bet.row;
    col = L::SectionX[bet.section] + L::SectionW[bet.section] +
          static_cast<int>(playerIndex) * L::PlayerStride;

    return true;
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::onBetMade(
    const Craps::PlayerId& playerId,
    const Craps::BetId&    betId,
    const BetName&         betName,
    Gen::Money             contractAmount,
    Gen::Money             oddsAmount,
    unsigned               pivot)
{
    (void)betId;

    const std::size_t playerIndex = getPlayerIndex(playerId);

    if (playerIndex >= players_.size())
        return;

    const std::string_view label =
        crapsBetNameToLabel(betName, pivot);

    if (label.empty())
        return;

    const auto betIt =
        std::find_if(
            bets_.begin(),
            bets_.end(),
            [&label](const Bet& bet)
            {
                return bet.label == label;
            });

    if (betIt == bets_.end())
        return;

    const std::size_t betIndex =
        static_cast<std::size_t>(
            std::distance(bets_.begin(), betIt));

    setBetState(
        betIndex,
        playerIndex,
        calcBetState(contractAmount, oddsAmount));

    drawBetMarker(betIndex, playerIndex);
}

//----------------------------------------------------------------

std::size_t
PlayerAreaAllPlayers::getPlayerIndex(
    const Craps::PlayerId& playerId) const
{
    for (std::size_t i = 0; i < players_.size(); ++i)
    {
        if (players_[i].playerId == playerId)
            return i;
    }

    return players_.size();
}

//----------------------------------------------------------------

PlayerAreaAllPlayers::BetState
PlayerAreaAllPlayers::calcBetState(
    Gen::Money contractAmount,
    Gen::Money oddsAmount) const
{
    assert(contractAmount > 0);

    return oddsAmount > 0
        ? BetState::BetWithOdds
        : BetState::Bet;
}

//----------------------------------------------------------------

std::string_view
PlayerAreaAllPlayers::crapsBetNameToLabel(
    const BetName& betName,
    unsigned pivot) const
{
    switch (betName)
    {
        case BetName::PassLine:
            return "PassLine";

        case BetName::Come:
            switch (pivot)
            {
                case 0:  return "Come";
                case 4:  return "Come4";
                case 5:  return "Come5";
                case 6:  return "Come6";
                case 8:  return "Come8";
                case 9:  return "Come9";
                case 10: return "Come10";
                default: return {};
            }

        case BetName::DontPass:
            return "DontPass";

        case BetName::DontCome:
            switch (pivot)
            {
                case 0:  return "DontCome";
                case 4:  return "DontCome4";
                case 5:  return "DontCome5";
                case 6:  return "DontCome6";
                case 8:  return "DontCome8";
                case 9:  return "DontCome9";
                case 10: return "DontCome10";
                default: return {};
            }

        case BetName::Place:
            switch (pivot)
            {
                case 4:  return "Place4";
                case 5:  return "Place5";
                case 6:  return "Place6";
                case 8:  return "Place8";
                case 9:  return "Place9";
                case 10: return "Place10";
                default: return {};
            }

        case BetName::Hardway:
            switch (pivot)
            {
                case 4:  return "Hard4";
                case 6:  return "Hard6";
                case 8:  return "Hard8";
                case 10: return "Hard10";
                default: return {};
            }

        case BetName::CandE:
            return "C&E";

        case BetName::Field:
            return "Field";

        case BetName::Buy:
            switch (pivot)
            {
                case 4:  return "Buy4";
                case 5:  return "Buy5";
                case 6:  return "Buy6";
                case 8:  return "Buy8";
                case 9:  return "Buy9";
                case 10: return "Buy10";
                default: return {};
            }

        case BetName::Lay:
            switch (pivot)
            {
                case 4:  return "Lay4";
                case 5:  return "Lay5";
                case 6:  return "Lay6";
                case 8:  return "Lay8";
                case 9:  return "Lay9";
                case 10: return "Lay10";
                default: return {};
            }

        case BetName::AnyCraps:
            return "AnyC";

        case BetName::AnySeven:
            return "Any7";

        case BetName::Horn:
            return "Horn";

        default:
            return {};
    }
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::onPlayerJoinedTable(
    const Craps::PlayerId& playerId)
{
    // Ignore duplicate join notifications.
    const auto alreadyPresent =
        std::find_if(
            players_.begin(),
            players_.end(),
            [&playerId](const Player& player)
            {
                return player.playerId == playerId;
            });

    if (alreadyPresent != players_.end())
        return;

    // The display has a fixed number of player columns.
    if (players_.size() >= LayoutAllPlayers::MaxPlayers)
    {
        Gen::Logger::instance().logWarn(
            "PlayerAreaAllPlayers: cannot display player; "
            "maximum player columns reached");
        return;
    }

    std::string name;
    Gen::ErrorPass ep;

    const auto rc =
        Ctrl::CrapsReaders::readPlayerName(playerId, name, ep);

    assert(rc == Gen::ReturnCode::Success);

    if (rc != Gen::ReturnCode::Success)
        return;

    const std::size_t playerIndex = players_.size();

    // ColorManager is expected to have assigned the color before
    // this window receives the join event.
    assert(ColorManager::instance().getPlayerColor(playerId).has_value());

    players_.push_back(
    {
        .playerId = playerId,
        .initial  = chooseInitial(name)
    });

    // A newly appended player's states are already None because Player's
    // bet-state arrays are independent of players_; explicitly clear the
    // new column in case this method is later changed to reuse columns.
    for (Bet& bet : bets_)
        bet.state[playerIndex] = BetState::None;

    drawPlayerHeaders();
    drawBetMarkers();
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::onPlayerLeftTable(
    const Craps::PlayerId& playerId)
{
    const auto playerIt =
        std::find_if(
            players_.begin(),
            players_.end(),
            [&playerId](const Player& player)
            {
                return player.playerId == playerId;
            });

    // Ignore stale or duplicate leave notifications.
    if (playerIt == players_.end())
        return;

    const std::size_t removedIndex =
        static_cast<std::size_t>(
            std::distance(players_.begin(), playerIt));

    players_.erase(playerIt);

    // Player bet states are stored by column index. Compact every
    // column after the removed player so the remaining players retain
    // their states.
    for (Bet& bet : bets_)
    {
        for (std::size_t index = removedIndex;
             index + 1 < LayoutAllPlayers::MaxPlayers;
             ++index)
        {
            bet.state[index] = bet.state[index + 1];
        }

        bet.state[LayoutAllPlayers::MaxPlayers - 1] =
            BetState::None;
    }

    drawPlayerHeaders();
    drawBetMarkers();
}

//----------------------------------------------------------------
