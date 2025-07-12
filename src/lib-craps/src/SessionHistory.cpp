//----------------------------------------------------------------
//
// File: SessionHistory.cpp
//
//----------------------------------------------------------------

#include <craps/SessionHistory.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <gen/Logger.h>

using namespace Craps;

//----------------------------------------------------------------

const SessionHistory::Sessions&
SessionHistory::getSessionHistory() const
{
    return sessions_;
}

//----------------------------------------------------------------
//
// Creates and appends a new summary record to SessionHistory.
// This is meant to be called before saving the history file.
//
void
SessionHistory::addNewSummary(
    unsigned numPlayers,
    Gbl::Money amtDeposited,
    Gbl::Money amtWithdrawn,
    const std::string& sessionStartStr,
    Gen::Timepoint::Duration duration)
{
    Summary sum;
    sum.amtIntake = amtDeposited;
    sum.amtPayout = amtWithdrawn;
    sum.duration  = duration;
    Gen::Timepoint tp(sessionStartStr);
    sum.date      = tp;
}

//----------------------------------------------------------------

void
SessionHistory::saveFile(const std::string& dir,
                         const std::string& tableId) const
{
    namespace fs = std::filesystem;
    fs::path path = fs::path(dir) / ("SessionHistory-" + tableId + ".yaml");
    LOG_DEBUG(lgr, "SessionHistory::saveFile(" + path.string()  + ")");
    std::ofstream fout(path);
    fout << toYAML();
}

//-----------------------------------------------------------------

void
SessionHistory::loadFile(const std::string& dir,
                         const std::string& tableId)
{
    namespace fs = std::filesystem;

    fs::path path = fs::path(dir) / ("SessionHistory-" + tableId + ".yaml");

    if (!fs::exists(path))
    {
        throw std::runtime_error("SessionHistory::loadFile() file does not exist: " + path.string());
    }

    std::ifstream fin(path);
    if (!fin.is_open())
    {
        throw std::runtime_error("SessionHistory::loadFile() Failed to open YAML file: " + path.string());
    }

    YAML::Node root = YAML::Load(fin);
    fromYAML(root);

}

//-----------------------------------------------------------------

YAML::Node 
SessionHistory::Summary::toYAML() const
{
    YAML::Node node;
    node["numBets"]    = numBets;
    node["amtIntake"]  = amtIntake;
    node["amtPayout"]  = amtPayout;
    node["numPlayers"] = numPlayers;
    node["date"]       = date.toString(); // As ISO date/time
    node["duration"]   = Gen::Timepoint::formatDurationWithDays(duration);
    return node;
}

//-----------------------------------------------------------------

void
SessionHistory::Summary::fromYAML(const YAML::Node& node)
{
    if (node["numBets"])    numBets    = node["numBets"].as<unsigned>();
    if (node["amtIntake"])  amtIntake  = node["amtIntake"].as<unsigned>();
    if (node["amtPayout"])  amtPayout  = node["amtPayout"].as<unsigned>();
    if (node["numPlayers"]) numPlayers = node["numPlayers"].as<unsigned>();
    if (node["date"])
    {
        std::string s = node["date"].as<std::string>();
        date = Gen::Timepoint::parseDurationWithDays(s);
    }
    if (node["duration"])
    {
        std::string s = node["duration"].as<std::string>();
        duration = Gen::Timepoint::parseDurationWithDays(s);
    }
}

//-----------------------------------------------------------------

YAML::Node
SessionHistory::toYAML() const
{
    YAML::Node node;
    for (const auto& session : sessions_)
    {
        node.push_back(session.toYAML());
    }
    return node;
}

//-----------------------------------------------------------------
//
// Repeatedly read summary records from file
//
void
SessionHistory::fromYAML(const YAML::Node& node)
{
    sessions_.clear();

    if (!node || !node.IsSequence())
    {
        throw std::runtime_error("SessionHistory::fromYAML: Expected a YAML sequence");
    }

    for (const auto& entry : node)
    {
        SessionHistory::Summary summary;
        summary.fromYAML(entry);
        sessions_.push_back(std::move(summary));
    }
}

//----------------------------------------------------------------
