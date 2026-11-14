//----------------------------------------------------------------
//
// File: RollStats.cpp
//
//----------------------------------------------------------------

#include <craps/RollStats.h>
#include <craps/CrapsTypes.h>

using namespace Craps;

//-----------------------------------------------------------------

unsigned
Counter::count() const
{
    return count_;
}

//-----------------------------------------------------------------

unsigned
Counter::repeats() const
{
    return maxRepeats;
}

//-----------------------------------------------------------------

void
Counter::bump()
{
    count_++;
    armed ? curRepeats++ : armed = true;
    maxRepeats = std::max(curRepeats, maxRepeats);
}

//-----------------------------------------------------------------

void
Counter::disarm()
{
    armed = false;
    curRepeats = 0;
}

//-----------------------------------------------------------------

void
Counter::reset()
{
    count_     = 0;
    armed      = false;
    curRepeats = 0;
    maxRepeats = 0;
    pivot      = 0;
}

//-----------------------------------------------------------------

void
Counter::merge(const Counter& session)
{
    count_ += session.count_;
    maxRepeats = std::max(maxRepeats, session.maxRepeats);
}

//-----------------------------------------------------------------

YAML::Node 
Counter::toYAML() const
{
    YAML::Node node;
    node["count"]       = count_;
    node["maxRepeats"]  = maxRepeats;
    return node;
}

//-----------------------------------------------------------------

void
Counter::fromYAML(const YAML::Node& node)
{
    count_     = node["count"].as<unsigned>();
    maxRepeats = node["maxRepeats"].as<unsigned>();
    curRepeats = 0;
    pivot = 0;
    armed = false;
}

//-----------------------------------------------------------------

void
PointCounts::reset()
{
    wins.reset();
    lose.reset();
}

//-----------------------------------------------------------------

void
PointCounts::merge(const PointCounts& sessionPc)
{
    wins.merge(sessionPc.wins);
    lose.merge(sessionPc.lose);
}

//-----------------------------------------------------------------

YAML::Node PointCounts::toYAML() const
{
    YAML::Node node;
    node["wins"] = wins.toYAML();
    node["lose"] = lose.toYAML();
    return node;
}

//-----------------------------------------------------------------

void
PointCounts::fromYAML(const YAML::Node& node)
{
    wins.fromYAML(node["wins"]);
    lose.fromYAML(node["lose"]);
}

//-----------------------------------------------------------------

void
RollStats::reset()
{
    for (unsigned i = 2; i <= 12; i++)
    {
        numberCounts[i].reset();
        pointNumberCounts[i].reset();
    }

    for (unsigned i : {4, 5, 6, 8, 9, 10})
    {
        passPntCnts[i].reset();
        dontPassPntCnts[i].reset();
        comePntCnts[i].reset();
        dontComePntCnts[i].reset();
        hardwayCounts[i].reset();
        hardwayEasy[i].reset();
        hardwaySeven[i].reset();
        anyEstPntCnts[i].reset();
    }

    numRolls = 0;
    numPointsEstablished = 0;
    comeOutRolls.reset();
    pointRolls.reset();
    passWinsComeOut.reset();
    passLoseComeOut.reset();
    dontPassWinsComeOut.reset();
    dontPassLoseComeOut.reset();
    comeWinsComeOut.reset();
    comeLoseComeOut.reset();
    dontComeWinsComeOut.reset();
    dontComeLoseComeOut.reset();
    fieldBetWins.reset();
    fieldBetLose.reset();
    sevenOuts.reset();
    shooterCounts.reset();
    twosOnComeOutRoll.reset();
    threesOnComeOutRoll.reset();
    sevensOnComeOutRoll.reset();
    elevensOnComeOutRoll.reset();
    twelvesOnComeOutRoll.reset();
    crapsOnComeOutRoll.reset();
    pointRepeaters.reset();
    placeRepeaters.reset();
}

//-----------------------------------------------------------------

void
RollStats::merge(const RollStats& session)
{
    for (unsigned i = 2; i <= 12; i++)
    {
        numberCounts[i].merge(session.numberCounts[i]);
        pointNumberCounts[i].merge(session.pointNumberCounts[i]);
    }

    for (unsigned i : {4, 5, 6, 8, 9, 10})
    {
        passPntCnts[i].merge(session.passPntCnts[i]);
        dontPassPntCnts[i].merge(session.dontPassPntCnts[i]);
        comePntCnts[i].merge(session.comePntCnts[i]);
        dontComePntCnts[i].merge(session.dontComePntCnts[i]);
        hardwayCounts[i].merge(session.hardwayCounts[i]);
        hardwayEasy[i].merge(session.hardwayEasy[i]);
        hardwaySeven[i].merge(session.hardwaySeven[i]);
        anyEstPntCnts[i].merge(session.anyEstPntCnts[i]);
    }
    
    numRolls += session.numRolls;
    numPointsEstablished += session.numPointsEstablished;
    comeOutRolls.merge(session.comeOutRolls);
    pointRolls.merge(session.pointRolls);
    passWinsComeOut.merge(session.passWinsComeOut);
    passLoseComeOut.merge(session.passLoseComeOut);
    dontPassWinsComeOut.merge(session.dontPassWinsComeOut);
    dontPassLoseComeOut.merge(session.dontPassLoseComeOut);
    comeWinsComeOut.merge(session.comeWinsComeOut);
    comeLoseComeOut.merge(session.comeLoseComeOut);
    dontComeWinsComeOut.merge(session.dontComeWinsComeOut);
    dontComeLoseComeOut.merge(session.dontComeLoseComeOut);
    fieldBetWins.merge(session.fieldBetWins);
    fieldBetLose.merge(session.fieldBetLose);
    sevenOuts.merge(session.sevenOuts);
    shooterCounts.merge(session.shooterCounts);
    twosOnComeOutRoll.merge(session.twosOnComeOutRoll);
    threesOnComeOutRoll.merge(session.threesOnComeOutRoll);
    sevensOnComeOutRoll.merge(session.sevensOnComeOutRoll);
    elevensOnComeOutRoll.merge(session.elevensOnComeOutRoll);
    twelvesOnComeOutRoll.merge(session.twelvesOnComeOutRoll);
    crapsOnComeOutRoll.merge(session.crapsOnComeOutRoll);
    pointRepeaters.merge(session.pointRepeaters);
    placeRepeaters.merge(session.placeRepeaters);
}

//-----------------------------------------------------------------

YAML::Node 
RollStats::toYAML() const
{
    YAML::Node node;
    numberCountsToYAML(node);
    pointCountsToYAML (node);
    singleCountsToYAML(node);
    node["pointRepeaters"] = pointRepeaters.toYAML();
    node["placeRepeaters"] = placeRepeaters.toYAML();
    return node;
}

//-----------------------------------------------------------------

void
RollStats::numberCountsToYAML(YAML::Node& node) const
{
    // Serialize numberCounts[2-12]
    YAML::Node numberNode;
    for (unsigned i = 2; i <= 12; ++i)
    {
        numberNode[std::to_string(i)] = numberCounts[i].toYAML();
    }
    node["numberCounts"] = numberNode;

    // Serialize anyEstPntCnts[4,5,6,8,9,10]
    YAML::Node estNode;
    for (unsigned i : {4, 5, 6, 8, 9, 10})
    {
        estNode[std::to_string(i)] = anyEstPntCnts[i].toYAML();
    }
    node["anyEstPntCnts"] = estNode;
    
    // Helper lambda to populate YAML from an array
    auto fillPointCounts = [&](YAML::Node subnode, const std::array<PointCounts, 11>& arr)
    {
        const std::vector<unsigned> points = {4, 5, 6, 8, 9, 10};
        for (unsigned pt : points)
        {
            subnode[std::to_string(pt)] = arr[pt].toYAML();
        }
    };
}

//-----------------------------------------------------------------

void
RollStats::pointCountsToYAML(YAML::Node& node) const
{
    // Helper lambda to populate YAML from an array
    auto fillPointCounts = [&](YAML::Node subnode, const std::array<PointCounts, 11>& arr)
    {
        const std::vector<unsigned> points = {4, 5, 6, 8, 9, 10};
        for (unsigned pt : points)
        {
            subnode[std::to_string(pt)] = arr[pt].toYAML();
        }
    };
    
    fillPointCounts(node["passPntCnts"],     passPntCnts);
    fillPointCounts(node["dontPassPntCnts"], dontPassPntCnts);
    fillPointCounts(node["comePntCnts"],     comePntCnts);
    fillPointCounts(node["dontComePntCnts"], dontComePntCnts);
    
    // Similar for hardway points
    auto fillHardwayCounts = [&](YAML::Node subnode, const std::array<PointCounts, 11>& arr)
    {
        const std::vector<unsigned> points = {4, 6, 8, 10};
        for (unsigned pt : points)
        {
            subnode[std::to_string(pt)] = arr[pt].toYAML();
        }
    };

    fillHardwayCounts(node["hardwayCounts"], hardwayCounts);

    // Hardway losses by cause, and rolls made while a point was on.
    for (unsigned pt : {4, 6, 8, 10})
    {
        node["hardwayEasyCounts"][std::to_string(pt)]  = hardwayEasy[pt].toYAML();
        node["hardwaySevenCounts"][std::to_string(pt)] = hardwaySeven[pt].toYAML();
    }
    for (unsigned i = 2; i <= 12; ++i)
        node["pointNumberCounts"][std::to_string(i)] = pointNumberCounts[i].toYAML();
}
    
//-----------------------------------------------------------------

void
RollStats::singleCountsToYAML(YAML::Node& node) const
{
    node["numRolls"]             = numRolls;
    node["numPointsEstablished"] = numPointsEstablished;
    node["comeOutRolls"]         = comeOutRolls.toYAML();
    node["pointRolls"]           = pointRolls.toYAML();
    node["passWinsComeOut"]      = passWinsComeOut.toYAML();
    node["passLoseComeOut"]      = passLoseComeOut.toYAML();
    node["comeWinsComeOut"]      = comeWinsComeOut.toYAML();
    node["comeLoseComeOut"]      = comeLoseComeOut.toYAML();
    node["dontPassWinsComeOut"]  = dontPassWinsComeOut.toYAML();
    node["dontPassLoseComeOut"]  = dontPassLoseComeOut.toYAML();
    node["dontComeWinsComeOut"]  = dontComeWinsComeOut.toYAML();
    node["dontComeLoseComeOut"]  = dontComeLoseComeOut.toYAML();
    node["fieldBetWins"]         = fieldBetWins.toYAML();
    node["fieldBetLose"]         = fieldBetLose.toYAML();
    node["sevenOuts"]            = sevenOuts.toYAML();
    node["shooterCounts"]        = shooterCounts.toYAML();
    node["twosOnComeOutRoll"]    = twosOnComeOutRoll.toYAML();
    node["threesOnComeOutRoll"]  = threesOnComeOutRoll.toYAML();
    node["sevensOnComeOutRoll"]  = sevensOnComeOutRoll.toYAML();
    node["elevensOnComeOutRoll"] = elevensOnComeOutRoll.toYAML();
    node["twelvesOnComeOutRoll"] = twelvesOnComeOutRoll.toYAML();
    node["crapsOnComeOutRoll"]   = crapsOnComeOutRoll.toYAML();
}
    
//-----------------------------------------------------------------

void
RollStats::fromYAML(const YAML::Node& node)
{
    numberCountsFromYAML(node);
    pointCountsFromYAML (node);
    singleCountsFromYAML(node);
    pointRepeaters.fromYAML(node["pointRepeaters"]);
    placeRepeaters.fromYAML(node["placeRepeaters"]);
}

//-----------------------------------------------------------------

void
RollStats::numberCountsFromYAML(const YAML::Node& node)
{
    // Deserialize numberCounts[2-12]
    const YAML::Node numberNode = node["numberCounts"];
    for (unsigned i = 2; i <= 12; ++i)
    {
        numberCounts[i].fromYAML(numberNode[std::to_string(i)]);
    }

    // Deserialize anyEstPntCnts[4,5,6,8,9,10]
    const YAML::Node estNode = node["anyEstPntCnts"];
    for (unsigned i : {4, 5, 6, 8, 9, 10})
    {
        anyEstPntCnts[i].fromYAML(estNode[std::to_string(i)]);
    }
}

//-----------------------------------------------------------------

void
RollStats::pointCountsFromYAML (const YAML::Node& node)
{
    // Helper lambda to load YAML into an array
    auto loadCounts = [&](const YAML::Node& groupNode,
                          std::array<PointCounts, 11>& arr,
                          const std::vector<unsigned>& points)
    {
        for (unsigned pt : points)
        {
            arr[pt].fromYAML(groupNode[std::to_string(pt)]);
        }
    };

    const std::vector<unsigned> points   = {4, 5, 6, 8, 9, 10};
    const std::vector<unsigned> hardways = {4, 6, 8, 10};
    loadCounts(node["passPntCnts"],     passPntCnts,     points);
    loadCounts(node["dontPassPntCnts"], dontPassPntCnts, points);
    loadCounts(node["comePntCnts"],     comePntCnts,     points);
    loadCounts(node["dontComePntCnts"], dontComePntCnts, points);
    loadCounts(node["hardwayCounts"],   hardwayCounts,   hardways);

    for (unsigned pt : {4, 6, 8, 10})
    {
        const std::string key = std::to_string(pt);
        hardwayEasy[pt].fromYAML(node["hardwayEasyCounts"][key]);
        hardwaySeven[pt].fromYAML(node["hardwaySevenCounts"][key]);
    }
    for (unsigned i = 2; i <= 12; ++i)
        pointNumberCounts[i].fromYAML(node["pointNumberCounts"][std::to_string(i)]);
}

//-----------------------------------------------------------------

void
RollStats::singleCountsFromYAML(const YAML::Node& node)
{
    numRolls                    = node["numRolls"].as<unsigned>();
    numPointsEstablished        = node["numPointsEstablished"].as<unsigned>();
    comeOutRolls.fromYAML        (node["comeOutRolls"]);
    pointRolls.fromYAML          (node["pointRolls"]);
    passWinsComeOut.fromYAML     (node["passWinsComeOut"]);
    passLoseComeOut.fromYAML     (node["passLoseComeOut"]);
    comeWinsComeOut.fromYAML     (node["comeWinsComeOut"]);
    comeLoseComeOut.fromYAML     (node["comeLoseComeOut"]);
    dontPassWinsComeOut.fromYAML (node["dontPassWinsComeOut"]);
    dontPassLoseComeOut.fromYAML (node["dontPassLoseComeOut"]);
    dontComeWinsComeOut.fromYAML (node["dontComeWinsComeOut"]);
    dontComeLoseComeOut.fromYAML (node["dontComeLoseComeOut"]);
    fieldBetWins.fromYAML        (node["fieldBetWins"]);
    fieldBetLose.fromYAML        (node["fieldBetLose"]);
    sevenOuts.fromYAML           (node["sevenOuts"]);
    shooterCounts.fromYAML       (node["shooterCounts"]);
    twosOnComeOutRoll.fromYAML   (node["twosOnComeOutRoll"]);
    threesOnComeOutRoll.fromYAML (node["threesOnComeOutRoll"]);
    sevensOnComeOutRoll.fromYAML (node["sevensOnComeOutRoll"]);
    elevensOnComeOutRoll.fromYAML(node["elevensOnComeOutRoll"]);
    twelvesOnComeOutRoll.fromYAML(node["twelvesOnComeOutRoll"]);
    crapsOnComeOutRoll.fromYAML  (node["crapsOnComeOutRoll"]);
}

//-----------------------------------------------------------------

//-----------------------------------------------------------------
//
// How often each bet type would have won or lost given the rolls seen,
// regardless of whether anyone bet. pivot == 0 means all points combined.
//
WinLossCounts
RollStats::simulatedCounts(BetName betName, unsigned pivot) const
{
    const RollStats& stats = *this;
    WinLossCounts result;

    const auto addPointCounts = [&](const PointCounts& counts)
    {
        result.wins   += counts.wins.count();
        result.losses += counts.lose.count();
    };

    const auto addAllPointCounts =
        [&](const std::array<PointCounts, 11>& counts)
    {
        for (unsigned point : {4U, 5U, 6U, 8U, 9U, 10U})
            addPointCounts(counts[point]);
    };

    const auto countByNumber = [&](auto isWin)
    {
        for (unsigned number = 2; number <= 12; ++number)
        {
            const unsigned count = stats.numberCounts[number].count();

            if (isWin(number))
                result.wins += count;
            else
                result.losses += count;
        }
    };

    switch (betName)
    {
    case BetName::PassLine:
        if (pivot != 0)
        {
            if (pivot < stats.passPntCnts.size())
                addPointCounts(stats.passPntCnts[pivot]);
        }
        else
        {
            result.wins   += stats.passWinsComeOut.count();
            result.losses += stats.passLoseComeOut.count();
            addAllPointCounts(stats.passPntCnts);
        }
        break;

    case BetName::DontPass:
        if (pivot != 0)
        {
            if (pivot < stats.dontPassPntCnts.size())
                addPointCounts(stats.dontPassPntCnts[pivot]);
        }
        else
        {
            result.wins   += stats.dontPassWinsComeOut.count();
            result.losses += stats.dontPassLoseComeOut.count();
            addAllPointCounts(stats.dontPassPntCnts);
        }
        break;

    case BetName::Come:
        if (pivot != 0)
        {
            if (pivot < stats.comePntCnts.size())
                addPointCounts(stats.comePntCnts[pivot]);
        }
        else
        {
            result.wins   += stats.comeWinsComeOut.count();
            result.losses += stats.comeLoseComeOut.count();
            addAllPointCounts(stats.comePntCnts);
        }
        break;

    case BetName::DontCome:
        if (pivot != 0)
        {
            if (pivot < stats.dontComePntCnts.size())
                addPointCounts(stats.dontComePntCnts[pivot]);
        }
        else
        {
            result.wins   += stats.dontComeWinsComeOut.count();
            result.losses += stats.dontComeLoseComeOut.count();
            addAllPointCounts(stats.dontComePntCnts);
        }
        break;

    // Place, Buy and Lay are off on the come out roll by default, so only
    // rolls made while a point was on are considered.
    case BetName::Place:
    case BetName::Buy:
        if (pivot >= 2 && pivot <= 12 && pivot != 7)
        {
            result.wins   = stats.pointNumberCounts[pivot].count();
            result.losses = stats.pointNumberCounts[7].count();
        }
        break;

    case BetName::Lay:   // Opposite of Buy: wins on 7, loses on the number
        if (pivot >= 2 && pivot <= 12 && pivot != 7)
        {
            result.wins   = stats.pointNumberCounts[7].count();
            result.losses = stats.pointNumberCounts[pivot].count();
        }
        break;

    case BetName::Hardway:
        if (pivot < stats.hardwayCounts.size())
            addPointCounts(stats.hardwayCounts[pivot]);
        break;

    case BetName::Field:
        result.wins   = stats.fieldBetWins.count();
        result.losses = stats.fieldBetLose.count();
        break;

    case BetName::AnyCraps:
        countByNumber([](unsigned n)
        {
            return CrapsNums.contains(n);
        });
        break;

    case BetName::AnySeven:
        countByNumber([](unsigned n)
        {
            return n == 7;
        });
        break;

    case BetName::Any711:
        countByNumber([](unsigned n)
        {
            return NaturalNums.contains(n);
        });
        break;

    case BetName::CandE:
    case BetName::Horn:
        countByNumber([](unsigned n)
        {
            return BookEnds.contains(n);
        });
        break;

    case BetName::World:
        countByNumber([](unsigned n)
        {
            return WorldNums.contains(n);
        });
        break;

    default:
        break;
    }

    return result;
}

//-----------------------------------------------------------------
//
// Simulated hardway losses for one hardway number: easy roll of that
// number versus a seven. Hardways are assumed to work on come out rolls.
//
HardwayLossCounts
RollStats::simulatedHardwayLosses(unsigned pivot) const
{
    HardwayLossCounts result;
    if (pivot < hardwayEasy.size())
    {
        result.easy  = hardwayEasy[pivot].count();
        result.seven = hardwaySeven[pivot].count();
    }
    return result;
}
