//----------------------------------------------------------------
//
// File: PlayerManifest.h
//
//----------------------------------------------------------------

#pragma once

#include <vector>
#include <craps/CrapsTypes.h>
#include <yaml-cpp/yaml.h>

namespace Ctrl {

class PlayerManifest
{
public:
    struct PlayerInfo
    {
        Craps::PlayerId playerId;
        std::string playerName;
        std::string shortDescription;
        std::string fullDescription;
        std::string fileName;
        
        YAML::Node toYAML() const;
        void fromYAML(const YAML::Node& node);
    };

    Craps::PlayerId getPlayerId(const std::string& fileName) const;
    std::string getFileName(const Craps::PlayerId& playerId) const;
    void loadFromFile();  // throws
    const std::vector<PlayerManifest::PlayerInfo>& getPlayers() const;
    void fromYAML(const YAML::Node& node);

private:
    std::vector<PlayerInfo> players_;
};

/*-----------------------------------------------------------*//**

@class PlayerManifest

@brief Reads in players.yaml, populates players_ vector.

*/

} // namespace Ctrl

//----------------------------------------------------------------
