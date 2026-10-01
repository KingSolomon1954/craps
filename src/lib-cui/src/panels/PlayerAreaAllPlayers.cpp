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
        wchar_t initial = players_[i].initial;
        if (i >= players_.size())
        {
            initial = Layout::DoubleDash;
        }
        drawWideCharacter(row, startCol, initial,
                          players_[i].colorPair);
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
        drawBetMarker(b.row,
                      b.detail.col,
                      b.detail.state, 
                      players_.colorPair);
    }
}

#if 0        
        for (std::size_t i = 0; i < b.detail.size(); i++)
        {
            LOG_DEBUG("b.row:" + std::to_string(b.row) +
                " col:" + std::to_string(b.detail[i].col) +
                " state:skip " +
                "colorpair:" + std::to_string(players_[i].colorPair));
    
            drawBetMarker(b.row,
                          b.detail[i].col,
                          b.detail[i].state, 
                          players_[i].colorPair);
        }
    }
}
#endif

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

std::string_view
PlayerAreaAllPlayers::crapsBetNameToLabel(BetName betName,
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
    assert(playerIndex < players_.size());

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
PlayerAreaAllPlayers::onBetResolved()
{
    // TODO
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
