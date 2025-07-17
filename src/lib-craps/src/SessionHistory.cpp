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
#include <gen/Debug.h>
#include <gen/Logger.h>

using namespace Craps;

//----------------------------------------------------------------

void
SessionHistory::setMaxSessions(size_t maxSessions)
{
    maxSessions_ = maxSessions;
}

//----------------------------------------------------------------

size_t
SessionHistory::getMaxSessions() const
{
    return maxSessions_;
}

//----------------------------------------------------------------

size_t
SessionHistory::getSessionCount() const
{
    return sessions_.size();
}

//----------------------------------------------------------------

unsigned
SessionHistory::getNumSessionsAlltime() const
{
    return numSessionsAlltime_;
}

//----------------------------------------------------------------

Gen::Timepoint
SessionHistory::getCurSessionStartTime() const
{
    return curSessionStartTime_;
}

//----------------------------------------------------------------

Gen::Timepoint
SessionHistory::getFirstSessionDate() const
{
    return firstSessionDate_;
}

//----------------------------------------------------------------

Gen::Timepoint::Duration
SessionHistory::getLongestSessionAlltime() const
{
    return longestSessionAlltime_;
}

//----------------------------------------------------------------

const SessionHistory::Sessions&
SessionHistory::getSessions() const
{
    return sessions_;
}

//-----------------------------------------------------------------
//
// Creates and writes new entry for current session.
//
// Call this before saving to file as program is known to be exiting.
// 
void
SessionHistory::addSessionSummary(
    unsigned numPlayers,
    unsigned numBets,
    Gen::Money amtDeposited,
    Gen::Money amtWithdrawn)
{
    Summary sum;
    sum.numPlayers = numPlayers;
    sum.numBets    = numBets;
    sum.amtIntake  = amtDeposited;
    sum.amtPayout  = amtWithdrawn;
    sum.date       = curSessionStartTime_;
    sum.duration   = curSessionStartTime_.sinceNow();
    sessions_.push_back(std::move(sum));
    
    longestSessionAlltime_ = std::max(sum.duration, longestSessionAlltime_);
    trimToMaxSessions();
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
    date        = Gen::Timepoint(node["date"].as<std::string>());
    duration    = Gen::Timepoint::parseDurationWithDays(node["duration"].as<std::string>());
    numBets     = node["numBets"].as<unsigned>(0);
    numPlayers  = node["numPlayers"].as<unsigned>(0);
    amtIntake   = node["amtIntake"].as<Gen::Money>(0);
    amtPayout   = node["amtPayout"].as<Gen::Money>(0);
}

//-----------------------------------------------------------------

void
SessionHistory::fromYAML(const YAML::Node& node)
{
    sessions_.clear();

    numSessionsAlltime_     = node["numSessionsAlltime"].as<unsigned>(0);
    firstSessionDate_       = Gen::Timepoint(node["firstSessionDate"].as<std::string>());
    longestSessionAlltime_  = Gen::Timepoint::parseDurationWithDays(node["longestSessionAlltime"].as<std::string>());

    const YAML::Node& hist = node["history"];
    if (hist && hist.IsSequence())
    {
        for (const auto& entry : hist)
        {
            Summary s;
            s.fromYAML(entry);
            sessions_.push_back(std::move(s));
        }
    }
    trimToMaxSessions();
}

//-----------------------------------------------------------------

YAML::Node
SessionHistory::toYAML() const
{
    YAML::Node node;

    node["numSessionsAlltime"]    = numSessionsAlltime_ + 1;
    node["firstSessionDate"]      = firstSessionDate_.toString();
    node["longestSessionAlltime"] = Gen::Timepoint::formatDurationWithDays(longestSessionAlltime_);

    YAML::Node hist;
    for (const auto& s : sessions_)
    {
        hist.push_back(s.toYAML());
    }

    node["history"] = hist;
    return node;
}

//-----------------------------------------------------------------

void
SessionHistory::trimToMaxSessions()
{
    if (sessions_.size() > maxSessions_)
    {
        const size_t surplus = sessions_.size() - maxSessions_;
        sessions_.erase(sessions_.begin(), sessions_.begin() + surplus);
    }
}

//-----------------------------------------------------------------
