//----------------------------------------------------------------
//
// File: PlayerAreaAllPlayers.cpp
//
//----------------------------------------------------------------

#include <cui/panels/PlayerAreaAllPlayers.h>
#include <cui/CuiStructs.h>
#include <controller/CrapsReaders.h>
#include <craps/CrapsTypes.h>
#include <gen/ErrorPass.h>
#include <gen/Logger.h>
#include <cassert>

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
PlayerAreaAllPlayers::init(WINDOW* pWin,
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
    addBet("PassLine",  row++, 0);
    addBet("Come",      row++, 0);
    addBet("Come4",     row++, 0);
    addBet("Come5",     row++, 0);
    addBet("Come6",     row++, 0);
    addBet("Come8",     row++, 0);
    addBet("Come9",     row++, 0);
    addBet("Come10",    row++, 0);
    addBet("DontPass",  row++, 0);
    addBet("DontCome",  row++, 0);
    addBet("DontCome4", row++, 0);
    addBet("DontCome5", row++, 0);
    addBet("DontCome6", row++, 0);
    addBet("DontCome8", row++, 0);
    addBet("DontCome9", row++, 0);
    addBet("DontCome10",row++, 0);

    row = Layout::FirstBetRow;

    // Section 1
    addBet("Place4",  row++, 1);
    addBet("Place5",  row++, 1);
    addBet("Place6",  row++, 1);
    addBet("Place8",  row++, 1);
    addBet("Place9",  row++, 1);
    addBet("Place10", row++, 1);
    addBet("Hard4",   row++, 1);
    addBet("Hard6",   row++, 1);
    addBet("Hard8",   row++, 1);
    addBet("Hard10",  row++, 1);
    addBet("C&E",     row++, 1);
    addBet("Field",   row++, 1);

    row = Layout::FirstBetRow;

    // Section 2
    addBet("Buy4",  row++, 2);
    addBet("Buy5",  row++, 2);
    addBet("Buy6",  row++, 2);
    addBet("Buy8",  row++, 2);
    addBet("Buy9",  row++, 2);
    addBet("Buy10", row++, 2);
    addBet("Lay4",  row++, 2);
    addBet("Lay5",  row++, 2);
    addBet("Lay6",  row++, 2);
    addBet("Lay8",  row++, 2);
    addBet("Lay9",  row++, 2);
    addBet("Lay10", row++, 2);
    addBet("AnyC",  row++, 2);
    addBet("Any7",  row++, 2);
    addBet("Horn",  row++, 2);
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

    assert(userIt != playerIds.end());

    orderedIds.push_back(userPlayerId);

    // Preserve the existing order for everyone else.
    for (const auto& pid : playerIds)
    {
        if (pid != userPlayerId)
            orderedIds.push_back(pid);

        if (orderedIds.size() == LayoutAllPlayers::MaxPlayers)
            break;
    }

    for (std::size_t i = 0; i < orderedIds.size(); ++i)
    {
        std::string name;

        rc = Ctrl::CrapsReaders::readPlayerName(
            orderedIds[i], name, ep);

        assert(rc == Gen::ReturnCode::Success);

        players_.push_back(
        {
            .initial  = chooseInitial(name),
            .colorPair = playerColorPair(i)
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
    static constexpr std::array<short,
        LayoutAllPlayers::MaxPlayers> colors =
    {
        ColorPairs::Player1,
        ColorPairs::Player2,
        ColorPairs::Player3,
        ColorPairs::Player4,
        ColorPairs::Player5,
        ColorPairs::Player6
    };

    assert(index < colors.size());
    return colors[index];
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::drawInternalBorders()
{
    using L = Layout;
    
    // Vertical lines
    mvwvline(pWin_, 0, L::col1_2,  0, winHeight_);
    mvwvline(pWin_, 0, L::col2_3,  0, winHeight_);
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
        for (int player = 0; player < LayoutAllPlayers::MaxPlayers; ++player)
        {
            const wchar_t initial =
                player < static_cast<int>(players_.size())
                    ? players_[player].initial : L::DoubleDash;

            const short colorPair = playerColorPair(player);

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
            bet.label.data(), static_cast<int>(bet.label.size()));
    }
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::drawBetMarkers()
{
    using L = Layout;
    
    for (const Bet& bet : bets_)
    {
        for (int player = 0; player < LayoutAllPlayers::MaxPlayers; ++player)
        {
            const int col =
                L::SectionX[bet.section] + L::SectionW[bet.section] +
                player * L::PlayerStride;

            wchar_t marker = L::Dot;
            short colorPair = 0;

            if (player < static_cast<int>(players_.size()))
            {
                colorPair = players_[player].colorPair;

                switch (bet.state[player])
                {
                    case BetState::None:
                        marker = L::Dot;
                        break;

                    case BetState::Bet:
                        marker = L::HollowCircle;
                        break;

                    case BetState::BetWithOdds:
                        marker = L::FilledCircle;
                        break;
                }
            }

            drawWideCharacter(bet.row, col, marker, colorPair);
        }
    }
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::drawWideCharacter(int row,
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
PlayerAreaAllPlayers::setBetState(std::size_t betIndex,
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
PlayerAreaAllPlayers::setBetState(std::string_view betName,
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
PlayerAreaAllPlayers::betPosition(std::size_t betIndex,
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
PlayerAreaAllPlayers::onBetMade()
{
    
    // TODO
    // setBetState();    
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::onPlayerJoinedTable()
{
    // TODO
}

//----------------------------------------------------------------

void
PlayerAreaAllPlayers::onPlayerLeftTable()
{
    // TODO
}

//----------------------------------------------------------------

