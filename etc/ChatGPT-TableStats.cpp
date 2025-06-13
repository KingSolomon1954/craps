#include "TableStats.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

TableStats::TableStats(const std::string& tableId_)
    : tableId(tableId_), sessionStart(std::chrono::system_clock::now()) {}

void TableStats::recordRoll(int shooterId, int rollValue) {
    rollsByShooter[shooterId].push_back(rollValue);
    if (rollValue >= 2 && rollValue <= 12)
        rollCounts[rollValue]++;
    totalRolls++;
}

void TableStats::recordWin(const std::string& betType) {
    betStats.wins[betType]++;
    totalWins++;
}

void TableStats::recordLoss(const std::string& betType) {
    betStats.losses[betType]++;
    totalLosses++;
}

void TableStats::merge(const TableStats& session) {
    totalRolls += session.totalRolls;
    totalWins += session.totalWins;
    totalLosses += session.totalLosses;

    for (int i = 2; i <= 12; ++i)
        rollCounts[i] += session.rollCounts[i];

    for (const auto& [bet, count] : session.betStats.wins)
        betStats.wins[bet] += count;

    for (const auto& [bet, count] : session.betStats.losses)
        betStats.losses[bet] += count;

    for (const auto& [shooter, rolls] : session.rollsByShooter)
        rollsByShooter[shooter].insert(
            rollsByShooter[shooter].end(), rolls.begin(), rolls.end());
}

YAML::Node TableStats::toYAML() const {
    YAML::Node node;
    node["tableId"] = tableId;
    node["sessionStart"] = timeToString(sessionStart);
    node["totalRolls"] = totalRolls;
    node["totalWins"] = totalWins;
    node["totalLosses"] = totalLosses;

    for (int i = 2; i <= 12; ++i)
        node["rollCounts"][i] = rollCounts[i];

    for (const auto& [bet, count] : betStats.wins)
        node["winsByBet"][bet] = count;

    for (const auto& [bet, count] : betStats.losses)
        node["lossesByBet"][bet] = count;

    for (const auto& [shooter, rolls] : rollsByShooter)
        node["rollsByShooter"][std::to_string(shooter)] = rolls;

    return node;
}

void TableStats::fromYAML(const YAML::Node& node) {
    tableId = node["tableId"].as<std::string>();
    sessionStart = stringToTime(node["sessionStart"].as<std::string>());
    totalRolls = node["totalRolls"].as<int>();
    totalWins = node["totalWins"].as<int>();
    totalLosses = node["totalLosses"].as<int>();

    for (const auto& it : node["rollCounts"])
        rollCounts[it.first.as<int>()] = it.second.as<int>();

    for (const auto& it : node["winsByBet"])
        betStats.wins[it.first.as<std::string>()] = it.second.as<int>();

    for (const auto& it : node["lossesByBet"])
        betStats.losses[it.first.as<std::string>()] = it.second.as<int>();

    for (const auto& it : node["rollsByShooter"]) {
        int shooter = std::stoi(it.first.as<std::string>());
        rollsByShooter[shooter] = it.second.as<std::vector<int>>();
    }
}

void TableStats::saveSession(const std::string& dir) const {
    auto timestamp = timeToString(sessionStart);
    fs::path path = fs::path(dir) / ("session-" + timestamp + "-" + tableId + ".yaml");
    std::ofstream fout(path);
    fout << toYAML();
}

void TableStats::saveAlltime(const std::string& dir) const {
    fs::path path = fs::path(dir) / ("alltime-" + tableId + ".yaml");
    std::ofstream fout(path);
    fout << toYAML();
}

std::string TableStats::timeToString(std::chrono::system_clock::time_point tp) {
    auto t = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "%Y-%m-%dT%H-%M-%S");
    return ss.str();
}

std::chrono::system_clock::time_point TableStats::stringToTime(const std::string& str) {
    std::tm tm = {};
    std::istringstream ss(str);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H-%M-%S");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}


