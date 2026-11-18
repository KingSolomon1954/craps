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
}

//----------------------------------------------------------------

std::size_t
PlayerAreaAllPlayers::removePlayer(const Craps::PlayerId& playerId)
{
    for (std::size_t i = 0; i < players_.size(); ++i)
    {
        if (players_[i].playerId == playerId)
        {
            players_.erase(players_.begin() + i);
            return i;
        }
    }
    return 0;
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

void
PlayerAreaAllPlayers::onBetMade(
    const Craps::PlayerId& playerId,
    Craps::BetId           betId,
    BetName                betName,
    Gen::Money             contractAmount,
    Gen::Money             oddsAmount,
    unsigned               pivot)
{
    (void)betId;

    const std::size_t playerIndex = getPlayerIndex(playerId);
    if (playerIndex >= players_.size())
    {
        // Can happen briefly when a player has just left the table.
        LOG_WARN("PlayerAreaAllPlayers::onBetMade(): player is not shown");
        return;
    }

    CuiUtils::CuiBetName cuiBetName =
        CuiUtils::betToCuiBetName(betName, pivot);

    std::size_t betIndex = getBetIndex(cuiBetName, playerIndex);

    Bet&    bet    = bets_[betIndex];
    Detail& detail = bet.detail[playerIndex];
        
    detail.state = calcBetState(contractAmount, oddsAmount);
    drawBetMarker(bet.row,
                  detail.col,
                  detail.state, 
                  players_[playerIndex].colorPair);
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
    onBetMade(playerId,
              betId,
              betName,
              contractAmount,
              oddsAmount,
              pivot);
}
    
//----------------------------------------------------------------

std::size_t
PlayerAreaAllPlayers::getBetIndex(CuiUtils::CuiBetName betName,
                                  std::size_t playerIndex) const
{
    for (std::size_t i = 0; i < bets_.size(); ++i)
    {
        if (bets_[i].betName == betName)
        {
            return i + playerIndex;
        }
    }
    return bets_.size();
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
    for (auto& bet : bets_)
    {
        for (std::size_t i = 0; i < bet.detail.size(); ++i)
        {
            if (bet.detail[i].betId == betId)
            {
                auto& d = bet.detail[i];  // convenience
                d.betId = 0;
                d.state = BetState::None;
                drawBetMarker(bet.row, d.col, d.state,
                              (i < players_.size()) ? players_[i].colorPair
                                                    : short(0));
                return;
            }
        }
    }
    LOG_ERROR("PlayerAreaAllPlayers::onBetResolved(): "
              "did not find the resolved bet on the screen.");
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::onPlayerJoinedTable(
    const Craps::PlayerId& playerId)
{
    for (std::size_t i = 0; i < players_.size(); ++i)
    {
        if (players_[i].playerId == playerId)  // Already have it
            return;  
    }

    if (players_.size() >= MaxPlayers)
    {
        Gen::Logger::instance().logWarn(
            "PlayerAreaOnePlayer: cannot display player; "
            "maximum players reached");
        return;
    }

    players_.push_back(
    {
        .playerId  = playerId,
        .initial   = chooseInitial(CuiUtils::playerName(playerId)),
        .colorPair = CuiUtils::playerColorPair(playerId),
    });

    drawPlayerHeaders();
    drawBetMarkers();
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::onPlayerLeftTable(
    const Craps::PlayerId& playerId)
{
    std::size_t idx = removePlayer(playerId);
    if (idx == 0) return;  // playerId wasn't found

    // Player bet states are stored by column index. Compact every
    // column after the removed player so the remaining players retain
    // their states.
    for (Bet& bet : bets_)
    {
        for (std::size_t i = idx; i + 1 < MaxPlayers; ++i)
        {
            bet.detail[i] = bet.detail[i + 1];
            bet.detail[i] = bet.detail[i + 1];
        }

        bet.detail[MaxPlayers - 1].state = BetState::None;
    }

    drawPlayerHeaders();
    drawBetMarkers();
}

//----------------------------------------------------------------
