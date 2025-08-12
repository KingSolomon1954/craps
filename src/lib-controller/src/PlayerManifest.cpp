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
#include <fstream>

using namespace Ctrl;

//----------------------------------------------------------------

const std::vector<PlayerManifest::PlayerInfo>&
PlayerManifest::getPlayers() const
{
    return players_;
}

//----------------------------------------------------------------

void
PlayerManifest::loadFromFile()
{
    std::string dir = Gbl::pConfigMgr->getString(
        Ctrl::ConfigManager::KeyDirsSysPlayers).value();

    namespace fs = std::filesystem;
    fs::path path = fs::path(dir) / ("players.yaml");

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
        PlayerInfo info;
        info.fromYAML(playerNode);
        players_.emplace_back(std::move(info));
    }
}

//-----------------------------------------------------------------

void
PlayerManifest::PlayerInfo::fromYAML(const YAML::Node& node)
{
    playerId         = node["playerId"].as<Craps::PlayerId>();
    playerName       = node["playerName"].as<std::string>();
    shortDescription = node["shortDescription"].as<std::string>();
    fullDescription  = node["fullDescription"].as<std::string>();
    fileName         = node["fileName"].as<std::string>();
}

//-----------------------------------------------------------------

YAML::Node
PlayerManifest::PlayerInfo::toYAML() const
{
    YAML::Node node;
    node["playerId"]         = playerId;
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
