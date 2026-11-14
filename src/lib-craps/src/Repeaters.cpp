//----------------------------------------------------------------
//
// File: Repeaters.cpp
//
//----------------------------------------------------------------

#include <craps/Repeaters.h>
#include <stdexcept>
#include <string>

using namespace Craps;

//----------------------------------------------------------------

Repeaters::Repeaters(const DiceNums& numbers)
{
    for (const unsigned n : numbers)
        buckets_[n];   // Create an empty entry
}

//----------------------------------------------------------------

void
Repeaters::hit(unsigned number)
{
    if (buckets_.contains(number))
        running_[number]++;
}

//----------------------------------------------------------------

void
Repeaters::endRun(bool recordZeros)
{
    for (auto& [number, buckets] : buckets_)
    {
        const auto it = running_.find(number);
        const unsigned hits = it == running_.end() ? 0 : it->second;
        if (hits > 0 || recordZeros)
            buckets[hits]++;
    }
    running_.clear();
}

//----------------------------------------------------------------
//
// Ends the run of just the given numbers. Other numbers keep counting.
//
void
Repeaters::closeRun(const DiceNums& numbers, bool recordZeros)
{
    for (const unsigned number : numbers)
    {
        const auto it = buckets_.find(number);
        if (it == buckets_.end())
            continue;
        const unsigned hits = running(number);
        if (hits > 0 || recordZeros)
            it->second[hits]++;
        running_.erase(number);
    }
}

//----------------------------------------------------------------

void
Repeaters::merge(const Repeaters& session)
{
    for (const auto& [number, buckets] : session.buckets_)
    {
        const auto mine = buckets_.find(number);
        if (mine == buckets_.end())
            continue;
        for (const auto& [hits, times] : buckets)
            mine->second[hits] += times;
    }
}

//----------------------------------------------------------------

void
Repeaters::reset()
{
    for (auto& entry : buckets_)
        entry.second.clear();
    running_.clear();
}

//----------------------------------------------------------------

const Repeaters::Buckets&
Repeaters::buckets(unsigned number) const
{
    static const Buckets empty;
    const auto it = buckets_.find(number);
    return it == buckets_.end() ? empty : it->second;
}

//----------------------------------------------------------------

unsigned
Repeaters::times(unsigned number, unsigned hits) const
{
    const auto& b = buckets(number);
    const auto it = b.find(hits);
    return it == b.end() ? 0 : it->second;
}

//----------------------------------------------------------------

unsigned
Repeaters::running(unsigned number) const
{
    const auto it = running_.find(number);
    return it == running_.end() ? 0 : it->second;
}

//----------------------------------------------------------------

bool
Repeaters::tracks(unsigned number) const
{
    return buckets_.contains(number);
}

//----------------------------------------------------------------
//
// Running tallies are transient, so they do not take part.
//
bool
Repeaters::operator==(const Repeaters& other) const
{
    return buckets_ == other.buckets_;
}

//----------------------------------------------------------------

YAML::Node
Repeaters::toYAML() const
{
    YAML::Node node(YAML::NodeType::Map);
    for (const auto& [number, buckets] : buckets_)
    {
        YAML::Node entry(YAML::NodeType::Map);
        for (const auto& [hits, times] : buckets)
            entry[std::to_string(hits)] = times;
        node[std::to_string(number)] = entry;
    }
    return node;
}

//----------------------------------------------------------------

void
Repeaters::fromYAML(const YAML::Node& node)
{
    if (!node || !node.IsMap())
        throw std::runtime_error("Repeaters YAML must be a map");

    for (auto& entry : buckets_)
    {
        const YAML::Node numberNode = node[std::to_string(entry.first)];
        if (!numberNode || !numberNode.IsMap())
            throw std::runtime_error("Repeaters YAML missing number " +
                                     std::to_string(entry.first));
        entry.second.clear();
        for (auto it = numberNode.begin(); it != numberNode.end(); ++it)
            entry.second[it->first.as<unsigned>()] = it->second.as<unsigned>();
    }
    running_.clear();
}

//----------------------------------------------------------------
