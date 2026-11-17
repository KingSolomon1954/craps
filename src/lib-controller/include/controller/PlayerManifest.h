//----------------------------------------------------------------
//
// File: PlayerManifest.h
//
//----------------------------------------------------------------

#pragma once

#include <filesystem>
#include <ostream>
#include <string>
#include <vector>
#include <craps/CrapsTypes.h>
#include <yaml-cpp/yaml.h>

namespace Ctrl {

/// Who a player is for. A user player can become the user player; a
/// buddy player is delivered with the game and only ever sits at the table.
enum class PlayerKind { User, Buddy };

const char* toString(PlayerKind kind);
PlayerKind  playerKindFromString(const std::string& s);   // throws

/// Lets doctest show a PlayerKind when a check fails.
inline std::ostream& operator<<(std::ostream& os, PlayerKind kind)
{
    return os << toString(kind);
}

class PlayerManifest
{
public:
    struct PlayerDescription
    {
        Craps::PlayerId playerId;
        PlayerKind kind = PlayerKind::User;
        std::string playerName;
        std::string shortDescription;
        std::string fullDescription;
        std::string fileName;
        
        YAML::Node toYAML() const;
        void fromYAML(const YAML::Node& node);
    };
    
    using PlayerList = std::vector<PlayerManifest::PlayerDescription>;

    Craps::PlayerId getPlayerId(const std::string& fileName) const;
    std::string getFileName(const Craps::PlayerId& playerId) const;
    const PlayerList& getPlayerList()                        const;
    const PlayerDescription* find(const Craps::PlayerId& playerId) const;
    const PlayerDescription* findByName(const std::string& name) const; // case-insensitive
    bool fileNameInUse(const std::string& fileName) const;
    void loadFromFile();  // throws
    void fromYAML(const YAML::Node& node);
    YAML::Node toYAML() const;

    /// @name Modifiers
    /// @{
    void add(PlayerDescription desc);
    bool remove(const Craps::PlayerId& playerId);
    bool rename(const Craps::PlayerId& playerId, const std::string& newName);
    void saveToFile() const;   // throws; atomic
    /// @}

private:
    PlayerList players_;
    std::filesystem::path path_;       // set by loadFromFile()
};

/*-----------------------------------------------------------*//**

@class PlayerManifest

@brief Reads and writes players.yaml, the list of known players.

Each entry has a kind, "user" or "buddy". The file is strict: an entry
without a kind is an error. saveToFile() writes to a temporary file and
renames it, so a failure never leaves a half-written manifest.

*/

} // namespace Ctrl

//----------------------------------------------------------------
