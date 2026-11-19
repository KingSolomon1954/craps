//----------------------------------------------------------------
//
// File: PlayerManifest.cpp
//
//----------------------------------------------------------------

#include <controller/PlayerManifest.h>
#include <controller/ConfigManager.h>
#include <controller/Globals.h>
#include <gen/ErrorPass.h>
#include <gen/FileUtils.h>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <system_error>

using namespace Ctrl;

namespace {

bool
sameNameIgnoringCase(const std::string& a, const std::string& b)
{
    return a.size() == b.size() &&
        std::equal(a.begin(), a.end(), b.begin(), [](char x, char y) {
            return std::tolower(static_cast<unsigned char>(x)) ==
                   std::tolower(static_cast<unsigned char>(y)); });
}

} // namespace

//----------------------------------------------------------------

const char*
Ctrl::toString(PlayerKind kind)
{
    return (kind == PlayerKind::Buddy) ? "buddy" : "user";
}

//----------------------------------------------------------------

PlayerKind
Ctrl::playerKindFromString(const std::string& s)
{
    if (s == "user")  return PlayerKind::User;
    if (s == "buddy") return PlayerKind::Buddy;
    throw std::runtime_error("invalid player kind \"" + s +
                             "\"; expected user or buddy");
}

//----------------------------------------------------------------

const PlayerManifest::PlayerList&
PlayerManifest::getPlayerList() const
{
    return players_;
}

//----------------------------------------------------------------

void
PlayerManifest::loadFromFile()
{
    std::string dir = Gbl::pConfigMgr->getString(
        Ctrl::ConfigManager::KeyDirsUsrPlayers).value();

    namespace fs = std::filesystem;
    fs::path path = fs::path(dir) / ("players.yaml");
    path_ = path;

    try
    {
        std::ifstream fin = Gen::FileUtils::openOrThrow(path);
        YAML::Node root = YAML::Load(fin);
        fromYAML(root["players"]);
    }
    catch (const std::runtime_error& e)
    {
        std::string diag("PlayerManifest::loadFromFile(): \"" + path.string() + "\"; ");
        throw std::runtime_error(diag + e.what());
    }
}

//-----------------------------------------------------------------

void
PlayerManifest::fromYAML(const YAML::Node& node)
{
    players_.clear();

    if (!node || !node.IsSequence())
    {
        throw std::runtime_error("PlayerManifest::fromYAML(): expected a sequence node");
    }

    for (const auto& playerNode : node)
    {
        PlayerDescription desc;
        desc.fromYAML(playerNode);
        players_.emplace_back(std::move(desc));
    }
}

//-----------------------------------------------------------------

void
PlayerManifest::PlayerDescription::fromYAML(const YAML::Node& node)
{
    playerId         = node["playerId"].as<Craps::PlayerId>();
    kind             = playerKindFromString(node["kind"].as<std::string>());
    playerName       = node["playerName"].as<std::string>();
    shortDescription = node["shortDescription"].as<std::string>();
    fullDescription  = node["fullDescription"].as<std::string>();
    fileName         = node["fileName"].as<std::string>();
}

//-----------------------------------------------------------------

YAML::Node
PlayerManifest::PlayerDescription::toYAML() const
{
    YAML::Node node;
    node["playerId"]         = playerId;
    node["kind"]             = std::string(toString(kind));
    node["playerName"]       = playerName;
    node["shortDescription"] = shortDescription;
    node["fullDescription"]  = fullDescription;
    node["fileName"]         = fileName;
    return node;
}

//-----------------------------------------------------------------

std::string
PlayerManifest::getFileName(const Craps::PlayerId& playerId) const
{
    for (const auto& p : players_)
    {
        if (p.playerId == playerId)
        {
            return p.fileName;
        }
    }
    return "";
}

//-----------------------------------------------------------------

Craps::PlayerId
PlayerManifest::getPlayerId(const std::string& fileName) const
{
    for (const auto& p : players_)
    {
        if (p.fileName == fileName)
        {
            return p.playerId;
        }
    }
    return "";
}

//-----------------------------------------------------------------

const PlayerManifest::PlayerDescription*
PlayerManifest::find(const Craps::PlayerId& playerId) const
{
    for (const auto& p : players_)
    {
        if (p.playerId == playerId)
        {
            return &p;
        }
    }
    return nullptr;
}

//-----------------------------------------------------------------

const PlayerManifest::PlayerDescription*
PlayerManifest::findByName(const std::string& name) const
{
    for (const auto& p : players_)
    {
        if (sameNameIgnoringCase(p.playerName, name))
        {
            return &p;
        }
    }
    return nullptr;
}

//-----------------------------------------------------------------

bool
PlayerManifest::fileNameInUse(const std::string& fileName) const
{
    for (const auto& p : players_)
    {
        if (p.fileName == fileName)
        {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------

void
PlayerManifest::add(PlayerDescription desc)
{
    players_.push_back(std::move(desc));
}

//-----------------------------------------------------------------

bool
PlayerManifest::remove(const Craps::PlayerId& playerId)
{
    auto it = std::find_if(players_.begin(), players_.end(),
        [&](const PlayerDescription& d) { return d.playerId == playerId; });
    if (it == players_.end())
    {
        return false;
    }
    players_.erase(it);
    return true;
}

//-----------------------------------------------------------------

bool
PlayerManifest::rename(const Craps::PlayerId& playerId,
                       const std::string& newName)
{
    for (auto& p : players_)
    {
        if (p.playerId == playerId)
        {
            p.playerName = newName;
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------

YAML::Node
PlayerManifest::toYAML() const
{
    YAML::Node seq(YAML::NodeType::Sequence);
    for (const auto& p : players_)
    {
        seq.push_back(p.toYAML());
    }
    YAML::Node root;
    root["players"] = seq;
    return root;
}

//-----------------------------------------------------------------
//
// Atomic: temporary file in the same directory, then rename.
//
void
PlayerManifest::saveToFile() const
{
    const std::string fname("PlayerManifest::saveToFile(): \"" +
                            path_.string() + "\"; ");
    if (path_.empty())
    {
        throw std::runtime_error(fname + "manifest was never loaded");
    }

    namespace fs = std::filesystem;
    fs::path tmp = path_;
    tmp += ".tmp";
    {
        std::ofstream fout(tmp, std::ios::trunc);
        if (!fout.is_open())
        {
            throw std::runtime_error(fname + "unable to open \"" +
                                     tmp.string() + "\" for writing");
        }
        fout << "#\n# Available Players.\n#\n" << toYAML() << "\n";
        fout.flush();
        if (!fout.good())
        {
            fout.close();
            std::error_code ignore;
            fs::remove(tmp, ignore);
            throw std::runtime_error(fname + "write failed");
        }
    }
    std::error_code ec;
    fs::rename(tmp, path_, ec);
    if (ec)
    {
        std::error_code ignore;
        fs::remove(tmp, ignore);
        throw std::runtime_error(fname + "rename failed: " + ec.message());
    }
}

//-----------------------------------------------------------------
