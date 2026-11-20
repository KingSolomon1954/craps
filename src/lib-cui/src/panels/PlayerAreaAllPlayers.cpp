//----------------------------------------------------------------
//
// File: PlayerAreaAllPlayers.cpp
//
//----------------------------------------------------------------

#include <cui/panels/PlayerAreaAllPlayers.h>
#include <cui/managers/ColorManager.h>
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
// Called by WindowPlayerArea after the base WINDOW pWin_ is created.
//
void
PlayerAreaAllPlayers::init(WINDOW* pWin)
{
    pWin_ = pWin;
    buildPlayerInfo();
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::buildPlayerInfo()
{
    auto orderedIds = CuiUtils::orderedPlayerIds();
    
    players_.clear();
    players_.reserve(MaxPlayers);

    for (const auto& playerId : orderedIds)
    {
        players_.push_back(
        {
            .playerId  = playerId,
            .initial   = chooseInitial(CuiUtils::playerName(playerId)),
            .colorPair = CuiUtils::playerColorPair(playerId),
        });
    }
}

//----------------------------------------------------------------

//
// Who is at the table, in what order or by what name has changed. Learn
// the players again, and carry each player's bet markers to his new
// column.
//
void
PlayerAreaAllPlayers::rebuild()
{
    const std::vector<Player> oldPlayers = players_;
    const auto oldBets = bets_;

    buildPlayerInfo();

    for (std::size_t row = 0; row < bets_.size(); ++row)
    {
        for (std::size_t col = 0; col < MaxPlayers; ++col)
        {
            Detail& d = bets_[row].detail[col];   // d.col never changes
            d.betId = 0;
            d.state = BetState::None;

            if (col >= players_.size()) continue;

            for (std::size_t old = 0; old < oldPlayers.size(); ++old)
            {
                if (oldPlayers[old].playerId == players_[col].playerId)
                {
                    d.betId = oldBets[row].detail[old].betId;
                    d.state = oldBets[row].detail[old].state;
                    break;
                }
            }
        }
    }

    resyncFromModel();
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
    drawLabels();
    drawBetMarkers();
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::drawPlayerHeaders()
{
    std::array<int, 3> sectionStartCol{12, 35, 56};

    for (std::size_t i = 0; i < sectionStartCol.size(); ++i)
    {
        drawSectionHeader(sectionStartCol[i]);
    }
}

//----------------------------------------------------------------

void    
PlayerAreaAllPlayers::drawSectionHeader(int startCol)
{
    const int row = 1;
    
    for (std::size_t i = 0; i < MaxPlayers; ++i)
    {
        wchar_t initial = Layout::DoubleDash;   // Empty seat
        short   pair    = 0;
        if (i < players_.size())
        {
            initial = players_[i].initial;
            pair    = players_[i].colorPair;
        }
        drawWideCharacter(row, startCol, initial, pair);
        startCol += 2;
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
PlayerAreaAllPlayers::drawLabels()
{
    for (const auto& label : labels_)
    {
        mvwaddstr(pWin_, label.row, label.col, label.label.data());
    }
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::drawBetMarkers()
{
    for (auto b : bets_)
    {
        for (std::size_t i = 0; i < b.detail.size(); i++)
        {
            const short pair =
                (i < players_.size()) ? players_[i].colorPair : 0;
            drawBetMarker(b.row,
                          b.detail[i].col,
                          b.detail[i].state, 
                          pair);
        }
    }
        
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::drawBetMarker(int row, int col,
                                    BetState state, short colorPair)
{
    auto marker = getMarker(state);
    if (colorPair != 0) wattron(pWin_, COLOR_PAIR(colorPair));
    wchar_t text[2] = { marker, L'\0' };
    mvwaddwstr(pWin_, row, col, text);
    if (colorPair != 0) wattroff(pWin_, COLOR_PAIR(colorPair));
}

//----------------------------------------------------------------

wchar_t 
PlayerAreaAllPlayers::getMarker(BetState s) const
{
    switch (s)
    {
        case BetState::None: return Layout::Dot;
        case BetState::Bet:  return Layout::HollowCircle;
        case BetState::BetWithOdds: return Layout::FilledCircle;
        default: assert(false); return Layout::DoubleDash;
    }
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

//
// Which screen cell holds a bet. One cell per bet; the marker says
// whether odds were taken. Returns bets_.size() if the bet has no cell.
//
// PassLine/DontPass keep their cell at any pivot (betToCuiBetName()
// would map a pivoted one to its odds cell, which this view lacks).
//
std::size_t
PlayerAreaAllPlayers::findBetRow(BetName betName, unsigned pivot) const
{
    using N = CuiUtils::CuiBetName;

    N name;
    switch (betName)
    {
        case BetName::PassLine: name = N::PassLine; break;
        case BetName::DontPass: name = N::DontPass; break;
        default: name = CuiUtils::betToCuiBetName(betName, pivot); break;
    }

    for (std::size_t i = 0; i < bets_.size(); ++i)
    {
        if (bets_[i].betName == name) return i;
    }
    return bets_.size();
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::clearBetId(Craps::BetId betId)
{
    for (auto& bet : bets_)
    {
        for (auto& d : bet.detail)
        {
            if (d.betId == betId)
            {
                d.betId = 0;
                d.state = BetState::None;
            }
        }
    }
}

//----------------------------------------------------------------
//
// Remember a bet. First forget where it was, because a bet can move
// (a Come bet travels to its number). Only remembers state. The
// caller redraws the window.
//
void
PlayerAreaAllPlayers::applyBet(const Craps::PlayerId& playerId,
                               Craps::BetId           betId,
                               BetName                betName,
                               Gen::Money             contractAmount,
                               Gen::Money             oddsAmount,
                               unsigned               pivot)
{
    clearBetId(betId);

    const std::size_t playerIndex = getPlayerIndex(playerId);
    if (playerIndex >= players_.size()) return;  // Not shown
    if (contractAmount == 0) return;

    const std::size_t row = findBetRow(betName, pivot);
    if (row >= bets_.size()) return;

    Detail& d = bets_[row].detail[playerIndex];
    d.betId   = betId;
    d.state   = calcBetState(contractAmount, oddsAmount);
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::onBetMade(
    const Craps::PlayerId& playerId,
    Craps::BetId           betId,
    BetName                betName,
    Gen::Money             contractAmount,
    Gen::Money             oddsAmount,
    unsigned               pivot)
{
    applyBet(playerId, betId, betName, contractAmount, oddsAmount, pivot);
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::onBetChanged(
    const Craps::PlayerId& playerId,
    Craps::BetId           betId,
    BetName                betName,
    Gen::Money             contractAmount,
    Gen::Money             oddsAmount,
    unsigned               pivot)
{
    applyBet(playerId, betId, betName, contractAmount, oddsAmount, pivot);
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::onBetResolved(
    const Craps::PlayerId& playerId,
    BetName                betName,
    Craps::BetId           betId,
    Gen::Money             amountWin,
    Gen::Money             amountLose)
{
    // A win does not mean the bet left the table (Place, Buy, Lay and
    // Hardway stay up; Come bets travel). The screen is re-synced from
    // the model in onResolveBetsEnd().
    (void)playerId; (void)betName; (void)betId;
    (void)amountWin; (void)amountLose;
}

//----------------------------------------------------------------
//
// The bet left the table (taken down, or finished, or its player left).
// The window redraw paints it.
//
void
PlayerAreaAllPlayers::onBetRemoved(const Craps::PlayerId& playerId,
                                   Craps::BetId           betId)
{
    (void)playerId;
    clearBetId(betId);
}

//----------------------------------------------------------------
//
// The roll is settled. Bets were removed or moved without any event,
// so learn every shown player's bets again from the model.
//
void
PlayerAreaAllPlayers::onResolveBetsEnd()
{
    resyncFromModel();
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::resyncFromModel()
{
    for (auto& bet : bets_)
    {
        for (auto& d : bet.detail)
        {
            d.betId = 0;
            d.state = BetState::None;
        }
    }

    for (const auto& player : players_)
    {
        Gen::ErrorPass ep;
        std::vector<Craps::BetId> betIds;
        if (Ctrl::CrapsReaders::readPlayerGetBets(
                player.playerId, betIds, ep) != Gen::ReturnCode::Success)
        {
            continue;
        }

        for (auto betId : betIds)
        {
            BetName    betName;
            unsigned   pivot = 0;
            Gen::Money contract = 0;
            Gen::Money odds     = 0;
            Gen::ErrorPass ep2;

            // A bet can disappear between the list and the reads
            if (Ctrl::CrapsReaders::readBetName(betId, betName, ep2)
                    != Gen::ReturnCode::Success ||
                Ctrl::CrapsReaders::readBetPivot(betId, pivot, ep2)
                    != Gen::ReturnCode::Success ||
                Ctrl::CrapsReaders::readBetContractAmount(betId, contract, ep2)
                    != Gen::ReturnCode::Success ||
                Ctrl::CrapsReaders::readBetOddsAmount(betId, odds, ep2)
                    != Gen::ReturnCode::Success)
            {
                continue;
            }
            applyBet(player.playerId, betId, betName, contract, odds, pivot);
        }
    }
}

//----------------------------------------------------------------
