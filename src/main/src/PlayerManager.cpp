//----------------------------------------------------------------
//
// File: PlayerManager.cpp
//
//----------------------------------------------------------------

Manage player with:
std::unordered_map<std::string, Player> players;

Add a new player:
std::string newId = generateUUID();
players[newId] = Player(newId, "Alice");

Access a player:
std::string id = "some-uuid";
if (players.count(id))
{
    std::cout << "Found: " << players[id].name << "\n";
}


Save players to file:
void savePlayers(const std::unordered_map<std::string, Player>& players) {
    std::ofstream out("players.txt");
    for (const auto& [id, player] : players) {
        out << player.serialize() << "\n";
    }
}

Load players from file;
std::unordered_map<std::string, Player> loadPlayers()
{
    std::unordered_map<std::string, Player> players;
    std::ifstream in("players.txt");
    std::string line;
    while (std::getline(in, line))
    {
        Player p = Player::deserialize(line);
        players[p.uuid] = p;
    }
    return players;
}
