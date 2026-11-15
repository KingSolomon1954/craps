//----------------------------------------------------------------
//
// File: UserState.cpp
//
//----------------------------------------------------------------

#include <controller/UserState.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <controller/ConfigManager.h>
#include <gen/MultiLayerConfig.h>
#include <yaml-cpp/yaml.h>

using namespace Ctrl;
namespace fs = std::filesystem;

/*-----------------------------------------------------------*//**

Reads state.yaml from the user state directory, if there is a usable
one, and puts its values in cfg. Never throws. A state file that is
unusable is reported on stderr and ignored.

@param[in] multiConfig
    Directory keys must be resolved.
@param[in,out] cfg
    The layer to receive table.startTable, table.userIsPlayer and
    table.startPlayers[0..5].
*/
void
UserState::loadInto(const Gen::MultiLayerConfig& multiConfig,
                    Gen::ConfigLayer& cfg)
{
    using K = ConfigManager;
    const fs::path file = fs::path(
        multiConfig.getString(K::KeyDirsUsrState).value()) / FileName;

    std::error_code ec;
    if (!fs::exists(file, ec))
    {
        return;
    }

    UserState state;
    std::string whyNot;
    const bool ok = read(file,
        multiConfig.getString(K::KeyDirsUsrTables).value(),
        multiConfig.getString(K::KeyDirsUsrPlayers).value(),
        state, whyNot);
    if (!ok)
    {
        std::cerr << "Ignoring " << file.string() << ": " << whyNot << "\n";
        return;
    }

    cfg.set(K::KeyTableStart,        state.table);
    cfg.set(K::KeyTableUserIsPlayer, state.userPlayer);

    const char* slots[MaxStartPlayers] = {
        K::KeyTablePlayer1, K::KeyTablePlayer2, K::KeyTablePlayer3,
        K::KeyTablePlayer4, K::KeyTablePlayer5, K::KeyTablePlayer6 };
    for (size_t i = 0; i < MaxStartPlayers; i++)
    {
        cfg.set(slots[i], i < state.players.size() ? state.players[i] : "");
    }
}

/*-----------------------------------------------------------*//**

Parses and validates a state file. Never throws.

@param[in] file
    Path of the state file.
@param[in] tablesDir
    User tables directory; the table file must exist there.
@param[in] playersDir
    User players directory; each player file must exist there.
@param[out] state
    Receives the contents when the return value is true.
@param[out] whyNot
    Receives the reason when the return value is false.
@return
    true if the file was readable and consistent.
*/
bool
UserState::read(const fs::path& file, const fs::path& tablesDir,
                const fs::path& playersDir, UserState& state,
                std::string& whyNot)
{
    try
    {
        const YAML::Node root = YAML::LoadFile(file.string());
        const YAML::Node t = root["table"];
        if (!t || !t.IsMap())
        {
            whyNot = "no table section";
            return false;
        }

        UserState s;
        s.table      = t["startTable"].as<std::string>();
        s.userPlayer = t["userIsPlayer"].as<std::string>();

        const YAML::Node sp = t["startPlayers"];
        if (!sp || !sp.IsSequence())
        {
            whyNot = "no startPlayers list";
            return false;
        }
        for (const auto& n : sp)
        {
            const std::string f = n.as<std::string>();
            if (!f.empty())
            {
                s.players.push_back(f);
            }
        }
        if (s.players.size() > MaxStartPlayers)
        {
            whyNot = "too many players";
            return false;
        }

        std::error_code ec;
        if (s.table.empty() || !fs::exists(tablesDir / s.table, ec))
        {
            whyNot = "table file \"" + s.table + "\" not found";
            return false;
        }
        for (const auto& p : s.players)
        {
            if (!fs::exists(playersDir / p, ec))
            {
                whyNot = "player file \"" + p + "\" not found";
                return false;
            }
        }
        if (std::find(s.players.begin(), s.players.end(), s.userPlayer)
            == s.players.end())
        {
            whyNot = "user player \"" + s.userPlayer +
                     "\" is not among the players";
            return false;
        }

        state = std::move(s);
        return true;
    }
    catch (const std::exception& e)
    {
        whyNot = e.what();
        return false;
    }
}

/*-----------------------------------------------------------*//**

Writes state.yaml into stateDir. The file is written to a temporary
name and renamed over the old one.

@throw std::runtime_error
    On failure.
*/
void
UserState::save(const fs::path& stateDir) const
{
    YAML::Node list(YAML::NodeType::Sequence);
    for (size_t i = 0; i < MaxStartPlayers; i++)
    {
        list.push_back(i < players.size() ? players[i] : std::string());
    }

    YAML::Node t;
    t["startTable"]   = table;
    t["userIsPlayer"] = userPlayer;
    t["startPlayers"] = list;

    YAML::Node root;
    root["table"] = t;

    YAML::Emitter out;
    out << YAML::Comment("Written by the game at quit. Safe to delete.");
    out << root;

    const fs::path path = stateDir / FileName;
    const fs::path tmp  = path.string() + ".tmp";
    {
        std::ofstream f(tmp);
        f << out.c_str() << "\n";
        if (!f)
        {
            throw std::runtime_error("UserState::save(): unable to write " +
                                     tmp.string());
        }
    }
    std::error_code ec;
    fs::rename(tmp, path, ec);
    if (ec)
    {
        throw std::runtime_error("UserState::save(): rename failed for " +
                                 path.string() + "; " + ec.message());
    }
}

//----------------------------------------------------------------
