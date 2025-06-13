#pragma once

#include <string>
#include <vector>
#include <map>
#include <array>
#include <chrono>
#include <yaml-cpp/yaml.h>

struct BetStats {
    std::map<std::string, int> wins;
    std::map<std::string, int> losses;
};

class TableStats {
public:
    explicit TableStats(const std::string& tableId);

    void recordRoll(int shooterId, int rollValue);
    void recordWin(const std::string& betType);
    void recordLoss(const std::string& betType);

    void merge(const TableStats& session);

    YAML::Node toYAML() const;
    void fromYAML(const YAML::Node& node);

    void saveSession(const std::string& dir) const;
    void saveAlltime(const std::string& dir) const;

private:
    std::string tableId;
    std::chrono::system_clock::time_point sessionStart;

    int totalRolls = 0;
    int totalWins = 0;
    int totalLosses = 0;

    std::array<int, 13> rollCounts{};  // Index 2-12 used
    BetStats betStats;
    std::map<int, std::vector<int>> rollsByShooter;

    static std::string timeToString(std::chrono::system_clock::time_point tp);
    static std::chrono::system_clock::time_point stringToTime(const std::string& str);
};
