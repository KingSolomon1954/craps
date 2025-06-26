//----------------------------------------------------------------
//
// File: RollStatsStruct.cpp
//
//----------------------------------------------------------------

#include <craps/RollStatsStruct.h>

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
    if (node["count"])      count_     = node["count"].as<unsigned>();
    if (node["maxRepeats"]) maxRepeats = node["maxRepeats"].as<unsigned>();
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
    if (node["wins"]) wins.fromYAML(node["wins"]);
    if (node["lose"]) lose.fromYAML(node["lose"]);
}

//-----------------------------------------------------------------

void
RollStats::reset()
{
    for (unsigned i = 2; i < 12; i++)
    {
        numberCounts[i].reset();
    }

    for (unsigned i : {4, 5, 6, 8, 9, 10})
    {
        passPntCnts[i].reset();
        dontPassPntCnts[i].reset();
        comePntCnts[i].reset();
        dontComePntCnts[i].reset();
        hardwayCounts[i].reset();
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
}

//-----------------------------------------------------------------

void
RollStats::merge(const RollStats& session)
{
    for (unsigned i = 2; i < 12; i++)
    {
        numberCounts[i].merge(session.numberCounts[i]);
    }

    for (unsigned i : {4, 5, 6, 8, 9, 10})
    {
        passPntCnts[i].merge(session.passPntCnts[i]);
        dontPassPntCnts[i].merge(session.dontPassPntCnts[i]);
        comePntCnts[i].merge(session.comePntCnts[i]);
        dontComePntCnts[i].merge(session.dontComePntCnts[i]);
        hardwayCounts[i].merge(session.hardwayCounts[i]);
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
}

//-----------------------------------------------------------------

YAML::Node 
RollStats::toYAML() const
{
    YAML::Node node;
    numberCountsToYAML(node);
    pointCountsToYAML (node);
    singleCountsToYAML(node);
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
}
    
//-----------------------------------------------------------------

void
RollStats::singleCountsToYAML(YAML::Node& node) const
{
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
    node["fieldBetLose"]         = fieldBetWins.toYAML();
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
}

//-----------------------------------------------------------------

void
RollStats::numberCountsFromYAML(const YAML::Node& node)
{
    // Deserialize numberCounts[2-12]
    if (node["numberCounts"])
    {
        const YAML::Node& numberNode = node["numberCounts"];
        for (unsigned i = 2; i <= 12; ++i)
        {
            std::string key = std::to_string(i);
            if (numberNode[key])
            {
                numberCounts[i].fromYAML(numberNode[key]);
            }
        }
    }

    // Deserialize anyEstPntCnts[4,5,6,8,9,10]
    if (node["anyEstPntCnts"])
    {
        const YAML::Node& estNode = node["anyEstPntCnts"];
        for (unsigned i : {4, 5, 6, 8, 9, 10})
        {
            std::string key = std::to_string(i);
            if (estNode[key])
            {
                anyEstPntCnts[i].fromYAML(estNode[key]);
            }
        }
    }
}

//-----------------------------------------------------------------

void
RollStats::pointCountsFromYAML (const YAML::Node& node)
{
    // Helper lambda to load YAML into an array
    auto loadPointCounts = [&](const YAML::Node& groupNode, std::array<PointCounts, 11>& arr)
    {
        const std::vector<unsigned> points = {4, 5, 6, 8, 9, 10};
        for (unsigned pt : points)
        {
            std::string key = std::to_string(pt);
            if (groupNode[key])
            {
                arr[pt].fromYAML(groupNode[key]);
            }
        }
    };

    if (node["passPntCnts"])     loadPointCounts(node["passPntCnts"],     passPntCnts);
    if (node["dontPassPntCnts"]) loadPointCounts(node["dontPassPntCnts"], dontPassPntCnts);
    if (node["comePntCnts"])     loadPointCounts(node["comePntCnts"],     comePntCnts);
    if (node["dontComePntCnts"]) loadPointCounts(node["dontComePntCnts"], dontComePntCnts);

    // Similar for hardways
    auto loadHardwayCounts = [&](const YAML::Node& groupNode, std::array<PointCounts, 11>& arr)
    {
        const std::vector<unsigned> points = {4, 6, 8, 10};
        for (unsigned pt : points)
        {
            std::string key = std::to_string(pt);
            if (groupNode[key])
            {
                arr[pt].fromYAML(groupNode[key]);
            }
        }
    };

    if (node["hardwayCounts"]) loadPointCounts(node["hardwayCounts"], hardwayCounts);
}

//-----------------------------------------------------------------

void
RollStats::singleCountsFromYAML(const YAML::Node& node)
{
    if (node["comeOutRolls"])         comeOutRolls.fromYAML        (node["comeOutRolls"]);
    if (node["pointRolls"])           pointRolls.fromYAML          (node["pointRolls"]);
    if (node["passWinsComeOut"])      passWinsComeOut.fromYAML     (node["passWinsComeOut"]);
    if (node["passLoseComeOut"])      passLoseComeOut.fromYAML     (node["passLoseComeOut"]);
    if (node["comeWinsComeOut"])      comeWinsComeOut.fromYAML     (node["comeWinsComeOut"]);
    if (node["comeLoseComeOut"])      comeLoseComeOut.fromYAML     (node["comeLoseComeOut"]);
    if (node["dontPassWinsComeOut"])  dontPassWinsComeOut.fromYAML (node["dontPassWinsComeOut"]);
    if (node["dontPassLoseComeOut"])  dontPassLoseComeOut.fromYAML (node["dontPassLoseComeOut"]);
    if (node["dontComeWinsComeOut"])  dontComeWinsComeOut.fromYAML (node["dontComeWinsComeOut"]);
    if (node["dontComeLoseComeOut"])  dontComeLoseComeOut.fromYAML (node["dontComeLoseComeOut"]);
    if (node["fieldBetWins"])         fieldBetWins.fromYAML        (node["fieldBetWins"]);
    if (node["fieldBetLose"])         fieldBetLose.fromYAML        (node["fieldBetLose"]);
    if (node["sevenOuts"])            sevenOuts.fromYAML           (node["sevenOuts"]);
    if (node["shooterCounts"])        shooterCounts.fromYAML       (node["shooterCounts"]);
    if (node["twosOnComeOutRoll"])    twosOnComeOutRoll.fromYAML   (node["twosOnComeOutRoll"]);
    if (node["threesOnComeOutRoll"])  threesOnComeOutRoll.fromYAML (node["threesOnComeOutRoll"]);
    if (node["sevensOnComeOutRoll"])  sevensOnComeOutRoll.fromYAML (node["sevensOnComeOutRoll"]);
    if (node["elevensOnComeOutRoll"]) elevensOnComeOutRoll.fromYAML(node["elevensOnComeOutRoll"]);
    if (node["twelvesOnComeOutRoll"]) twelvesOnComeOutRoll.fromYAML(node["twelvesOnComeOutRoll"]);
    if (node["crapsOnComeOutRoll"])   crapsOnComeOutRoll.fromYAML  (node["crapsOnComeOutRoll"]);
}

//-----------------------------------------------------------------
