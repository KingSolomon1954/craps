//----------------------------------------------------------------
//
// File: StartChoices.cpp
//
//----------------------------------------------------------------

#include <controller/StartChoices.h>
#include <algorithm>
#include <system_error>
#include <controller/ConfigManager.h>
#include <gen/Logger.h>
#include <gen/MultiLayerConfig.h>

using namespace Ctrl;
namespace fs = std::filesystem;

namespace {

bool
haveFile(const fs::path& dir, const std::string& name)
{
    std::error_code ec;
    return !name.empty() && fs::is_regular_file(dir / name, ec);
}

// Tells whether name may be used as a player of the wanted kind. Warns
// when not. kinds empty means no checking.
bool
kindOkay(const std::map<std::string, PlayerKind>& kinds,
         const std::string& name, PlayerKind want, const char* from)
{
    if (kinds.empty())
    {
        return true;
    }
    auto it = kinds.find(name);
    if (it == kinds.end())
    {
        LOG_WARN("Startup player \"" + name + "\" from " + from +
                 " is not listed in players.yaml");
        return false;
    }
    if (it->second != want)
    {
        LOG_WARN("Startup player \"" + name + "\" from " + from + " is a " +
                 toString(it->second) + " player, not a " + toString(want) +
                 " player");
        return false;
    }
    return true;
}

// Chooses the first source whose file exists.
std::string
pickOne(const char* what,
        const fs::path& dir,
        const std::map<std::string, PlayerKind>* kinds,
        const std::optional<std::string>& fromConfig,
        const std::optional<std::string>& fromState,
        const std::string& fromDefault)
{
    struct Source { const char* from; const std::optional<std::string>* val; };
    const std::optional<std::string> def = fromDefault;
    const Source sources[] = {
        {"config.yaml", &fromConfig},
        {"state.yaml",  &fromState},
        {"the packaged defaults", &def} };

    for (const auto& s : sources)
    {
        if (!s.val->has_value() || (*s.val)->empty())
        {
            continue;
        }
        if (haveFile(dir, **s.val) &&
            (kinds == nullptr ||
             kindOkay(*kinds, **s.val, PlayerKind::User, s.from)))
        {
            return **s.val;
        }
        else if (!haveFile(dir, **s.val))
        {
            LOG_WARN(std::string("Startup ") + what + " \"" + **s.val +
                     "\" from " + s.from + " not found in " + dir.string());
        }
    }
    return fromDefault;   // Install is broken; let loading it say so
}

std::optional<std::string>
nonEmpty(const std::string& s)
{
    return s.empty() ? std::nullopt : std::optional<std::string>(s);
}

std::vector<std::string>
readList(const Gen::ConfigLayer& layer, const std::string& base)
{
    std::vector<std::string> list;
    for (size_t i = 0; ; i++)
    {
        auto v = layer.get(base + "[" + std::to_string(i) + "]");
        if (!v)
        {
            break;
        }
        list.push_back(*v);
    }
    return list;
}

std::optional<bool>
readBool(const Gen::ConfigLayer& layer, const std::string& key)
{
    auto v = layer.get(key);
    if (!v)
    {
        return std::nullopt;
    }
    if (*v == "true")  return true;
    if (*v == "false") return false;
    LOG_WARN("Ignoring " + key + ": \"" + *v + "\" is not true or false");
    return std::nullopt;
}

// Reads which file is a user player and which a buddy. Empty if the
// manifest cannot be read; PlayerManager reports that problem later,
// with better words.
std::map<std::string, PlayerKind>
readKinds(const fs::path& manifest)
{
    std::map<std::string, PlayerKind> kinds;
    try
    {
        YAML::Node root = YAML::LoadFile(manifest.string());
        for (const auto& n : root["players"])
        {
            kinds[n["fileName"].as<std::string>()] =
                playerKindFromString(n["kind"].as<std::string>());
        }
    }
    catch (const std::exception& e)
    {
        LOG_WARN("Unable to read player kinds from " + manifest.string() +
                 "; " + e.what());
        kinds.clear();
    }
    return kinds;
}

} // namespace

/*-----------------------------------------------------------*//**

Decides the table, the user player and the buddies. See the class
description for the rules. Never throws.

*/
StartChoices::Result
StartChoices::resolve(const Request& r)
{
    Result res;

    std::optional<std::string> lastTable, lastUser;
    if (r.last)
    {
        lastTable = nonEmpty(r.last->table);
        lastUser  = nonEmpty(r.last->userPlayer);
    }

    res.table = pickOne("table", r.tablesDir, nullptr, r.table, lastTable,
                        r.defaultTable);
    res.userPlayer = pickOne("user player", r.playersDir, &r.kinds,
                             r.userPlayer, lastUser, r.defaultUserPlayer);

    if (r.useBuddies.has_value() && !*r.useBuddies)
    {
        return res;                 // The user turned buddies off
    }

    const char* from = "the packaged defaults";
    const std::vector<std::string>* source = &r.defaultBuddies;
    if (r.buddies && !r.buddies->empty())
    {
        from = "config.yaml";
        source = &*r.buddies;
    }
    else if (r.last)
    {
        from = "state.yaml";
        source = &r.last->buddies;
    }

    for (const auto& b : *source)
    {
        if (b.empty() || b == res.userPlayer ||
            std::find(res.buddies.begin(), res.buddies.end(), b)
                != res.buddies.end())
        {
            continue;
        }
        if (!haveFile(r.playersDir, b))
        {
            LOG_WARN("Startup buddy player \"" + b + "\" from " + from +
                     " not found in " + r.playersDir.string());
            continue;
        }
        if (!kindOkay(r.kinds, b, PlayerKind::Buddy, from))
        {
            continue;
        }
        if (res.buddies.size() >= UserState::MaxBuddies)
        {
            LOG_WARN("Startup buddy players from " + std::string(from) +
                     " limited to " + std::to_string(UserState::MaxBuddies));
            break;
        }
        res.buddies.push_back(b);
    }
    return res;
}

/*-----------------------------------------------------------*//**

Gathers what the user wrote (already loaded into cfg from sys-config.yaml
and config.yaml), what the last game left, and the packaged defaults;
decides; and stores the answer in cfg under the keys the rest of the
game reads.

@param[in] multiConfig
    Directory keys must be resolved.
@param[in,out] cfg
    The files layer, with sys-config.yaml and config.yaml loaded.
*/
void
StartChoices::apply(const Gen::MultiLayerConfig& multiConfig,
                    Gen::ConfigLayer& cfg)
{
    using K = ConfigManager;

    Request r;
    r.table      = nonEmpty(cfg.get(K::KeyTableStartWithTable).value_or(""));
    r.userPlayer = nonEmpty(cfg.get(K::KeyTableStartWithUser).value_or(""));
    r.useBuddies = readBool(cfg, K::KeyTableStartWithBuddies);
    r.buddies    = readList(cfg, K::KeyTableStartWithThese);
    r.last       = UserState::load(multiConfig);

    r.defaultTable      = cfg.get(K::KeyTableDefaultTable)
                              .value_or("las-vegas-strip.yaml");
    r.defaultUserPlayer = cfg.get(K::KeyTableDefaultUser)
                              .value_or("unnamed.yaml");
    r.defaultBuddies    = readList(cfg, K::KeyTableDefaultBuddies);

    r.tablesDir  = multiConfig.getString(K::KeyDirsUsrTables).value();
    r.playersDir = multiConfig.getString(K::KeyDirsUsrPlayers).value();
    r.kinds      = readKinds(r.playersDir / "players.yaml");

    const Result res = resolve(r);

    // Animation speed. config.yaml wins; else what the last game remembered.
    if (r.last && r.last->animationSpeed)
    {
        cfg.set(K::KeyAnimationSpeedRemembered, toString(*r.last->animationSpeed));
        if (!cfg.get(K::KeyAnimationSpeed))
        {
            cfg.set(K::KeyAnimationSpeed, toString(*r.last->animationSpeed));
        }
    }

    cfg.set(K::KeyTableStart,        res.table);
    cfg.set(K::KeyTableUserIsPlayer, res.userPlayer);

    const char* slots[UserState::MaxStartPlayers] = {
        K::KeyTablePlayer1, K::KeyTablePlayer2, K::KeyTablePlayer3,
        K::KeyTablePlayer4, K::KeyTablePlayer5, K::KeyTablePlayer6 };
    std::vector<std::string> all{res.userPlayer};
    all.insert(all.end(), res.buddies.begin(), res.buddies.end());
    for (size_t i = 0; i < UserState::MaxStartPlayers; i++)
    {
        cfg.set(slots[i], i < all.size() ? all[i] : std::string());
    }

    std::string buddies;
    for (const auto& b : res.buddies)
    {
        buddies += (buddies.empty() ? "" : ", ") + b;
    }
    LOG_INFO("Starting with table " + res.table + ", user player " +
             res.userPlayer + ", buddies: " +
             (buddies.empty() ? "none" : buddies));
}

//----------------------------------------------------------------
