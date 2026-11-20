//----------------------------------------------------------------
//
// File: UserState.cpp
//
//----------------------------------------------------------------

#include <controller/UserState.h>
#include <fstream>
#include <stdexcept>
#include <system_error>
#include <controller/ConfigManager.h>
#include <gen/Logger.h>
#include <gen/MultiLayerConfig.h>
#include <yaml-cpp/yaml.h>

using namespace Ctrl;
namespace fs = std::filesystem;

/*-----------------------------------------------------------*//**

Reads state.yaml from the user state directory.

@param[in] multiConfig
    Directory keys must be resolved.
@return
    The remembered state, or nothing if there is no state file or it
    can't be used (which is logged).
*/
std::optional<UserState>
UserState::load(const Gen::MultiLayerConfig& multiConfig)
{
    const fs::path file = fs::path(multiConfig.getString(
        ConfigManager::KeyDirsUsrState).value()) / FileName;

    std::error_code ec;
    if (!fs::exists(file, ec))
    {
        return std::nullopt;
    }

    UserState state;
    std::string whyNot;
    if (!read(file, state, whyNot))
    {
        LOG_WARN("Ignoring " + file.string() + ": " + whyNot);
        return std::nullopt;
    }
    LOG_INFO("Loaded " + file.string());
    return state;
}

/*-----------------------------------------------------------*//**

Parses a state file. Never throws.

@param[in] file
    Path of the state file.
@param[out] state
    Receives the contents when the return value is true.
@param[out] whyNot
    Receives the reason when the return value is false.
@return
    true if the file was readable and had the expected sections.
*/
bool
UserState::read(const fs::path& file, UserState& state, std::string& whyNot)
{
    try
    {
        const YAML::Node root = YAML::LoadFile(file.string());
        const YAML::Node last = root["last"];
        if (!last || !last.IsMap())
        {
            whyNot = "no \"last\" section";
            return false;
        }

        UserState s;
        s.table      = last["table"].as<std::string>();
        s.userPlayer = last["userPlayer"].as<std::string>();

        const YAML::Node b = last["buddyPlayers"];
        if (!b || !b.IsSequence())
        {
            whyNot = "no buddyPlayers list";
            return false;
        }
        for (const auto& n : b)
        {
            const std::string f = n.as<std::string>();
            if (!f.empty())
            {
                s.buddies.push_back(f);
            }
        }

        // Optional and forgiving: anything unusable is just not remembered
        const YAML::Node settings = root["settings"];
        if (settings && settings.IsMap() && settings["animationSpeed"])
        {
            try
            {
                s.animationSpeed = parseAnimationSpeed(
                    settings["animationSpeed"].as<std::string>());
            }
            catch (const std::exception&) {}
        }
        if (settings && settings.IsMap() && settings["rollHistoryFormat"])
        {
            try
            {
                s.rollHistoryFormat = parseRollHistoryFormat(
                    settings["rollHistoryFormat"].as<std::string>());
            }
            catch (const std::exception&) {}
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
    for (const auto& b : buddies)
    {
        list.push_back(b);
    }

    YAML::Node last;
    last["table"]        = table;
    last["userPlayer"]   = userPlayer;
    last["buddyPlayers"] = list;

    YAML::Node root;
    root["last"] = last;

    if (animationSpeed)
    {
        root["settings"]["animationSpeed"] = toString(*animationSpeed);
    }
    if (rollHistoryFormat)
    {
        root["settings"]["rollHistoryFormat"] = toString(*rollHistoryFormat);
    }

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
