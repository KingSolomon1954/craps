//----------------------------------------------------------------
//
// File: LoggerTest.cpp
//
//----------------------------------------------------------------

#include <gen/Logger.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <doctest/doctest.h>

using Gen::Logger;
namespace fs = std::filesystem;

namespace {

std::string
readFile(const fs::path& p)
{
    std::ifstream f(p);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

} // namespace

//----------------------------------------------------------------
//
// Logger is a singleton, so these checks use unique markers rather
// than exact file contents.
//
TEST_CASE("Logger: messages before the output file are kept and written first")
{
    Logger& log = Logger::instance();
    log.disableConsoleLogging();

    const fs::path dir = fs::temp_directory_path() /
        ("royalcraps-loggertest-" + std::to_string(::getpid()));
    fs::remove_all(dir);
    fs::create_directories(dir);
    const fs::path first  = dir / "first.log";
    const fs::path second = dir / "second.log";

    // Make sure any earlier test's file is not the destination
    // by giving the logger a throwaway file first, then testing
    // the switch.
    log.setOutputFile((dir / "scratch.log").string());

    log.logInfo("early-marker-one");        // goes to scratch.log
    log.setOutputFile(first.string());
    log.logInfo("after-marker-two");
    log.logWarn("after-marker-three");

    const std::string text = readFile(first);
    CHECK(text.find("early-marker-one") == std::string::npos);
    CHECK(text.find("after-marker-two")   != std::string::npos);
    CHECK(text.find("[WARN]")             != std::string::npos);
    CHECK(text.find("after-marker-two") < text.find("after-marker-three"));

    // Switching again: later messages go to the new file only.
    log.setOutputFile(second.string());
    log.logError("switched-marker-four");
    CHECK(readFile(second).find("switched-marker-four") != std::string::npos);
    CHECK(readFile(first).find("switched-marker-four")  == std::string::npos);

    CHECK(log.hasOutputFile());
    fs::remove_all(dir);
}

//----------------------------------------------------------------

TEST_CASE("Logger: failing to open a file keeps the current one")
{
    Logger& log = Logger::instance();
    log.disableConsoleLogging();

    const fs::path dir = fs::temp_directory_path() /
        ("royalcraps-loggertest2-" + std::to_string(::getpid()));
    fs::remove_all(dir);
    fs::create_directories(dir);
    log.setOutputFile((dir / "good.log").string());

    CHECK_THROWS_AS(log.setOutputFile((dir / "nodir" / "bad.log").string()),
                    std::runtime_error);

    log.logInfo("still-works-marker");
    CHECK(readFile(dir / "good.log").find("still-works-marker")
          != std::string::npos);
    fs::remove_all(dir);
}

//----------------------------------------------------------------
