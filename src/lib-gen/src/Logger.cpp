//----------------------------------------------------------------
//
// File: Logger.cpp
//
//----------------------------------------------------------------

#include <gen/Logger.h>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace Gen;

//----------------------------------------------------------------

Logger&
Logger::instance()
{
    static Logger instance;
    return instance;
}

//----------------------------------------------------------------

void
Logger::setOutputFile(const std::string& filename)
{
    out_.open(filename, std::ios::app);

    if (!out_.is_open())
    {
        int error_number = errno;

        std::string diag = "Unable to open '" + filename + "': " +
            std::strerror(error_number) + " errno: " +
            std::to_string(error_number);
        throw std::runtime_error(diag);
    }
}

//----------------------------------------------------------------

void
Logger::enableConsoleLogging()
{
    if (!consoleEnabled_) log(Level::Info, "Console logging enabled");
    consoleEnabled_ = true;
}

//----------------------------------------------------------------

void
Logger::disableConsoleLogging()
{
    if (consoleEnabled_) log(Level::Info, "Console logging disabled");
    consoleEnabled_ = false;
}

//----------------------------------------------------------------

void
Logger::logDebug(const std::string& msg)
{
    if (debugOn_)
    {
        log(Level::Debug, msg);
    }
}

//----------------------------------------------------------------

void
Logger::logTrace(const std::string& msg)
{
    if (traceOn_)
    {
        log(Level::Trace, msg);
    }
}

//----------------------------------------------------------------

void
Logger::logInfo(const std::string& msg)
{
    log(Level::Info,  msg);
}
//----------------------------------------------------------------

void
Logger::logWarn(const std::string& msg)
{
    log(Level::Warn,  msg);
}

//----------------------------------------------------------------

void
Logger::logError(const std::string& msg)
{
    log(Level::Error, msg);
}

//----------------------------------------------------------------
//
// Formatting looks like this:
//
// 2025-07-05 15:10:04.004 [INFO]  Starting Royal Craps v0.0.1-1751567802
// 2025-07-05 15:10:06.586 [DEBUG] TableStats::saveFile(/work/craps.log)
//
void
Logger::log(Level level, const std::string& message)
{
    assert(out_.is_open());  // User must have already set output file
           
    std::string ts = timestamp();
    std::string label = " [" + levelToString(level) + "] ";

    // Always write to file
    out_ << ts << std::left << std::setw(9) <<label << message << std::endl;

    if (consoleEnabled_)  // Write to console if enabled
    {
        std::cout << ts << levelColor(level)
               << std::left << std::setw(9) << label
               << rang::style::reset << message << std::endl;
    }
}

//----------------------------------------------------------------

std::string
Logger::timestamp() const
{
    using namespace std::chrono;

    auto now = system_clock::now();
    auto time_t_now = system_clock::to_time_t(now);
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &time_t_now);
#else
    localtime_r(&time_t_now, &tm);
#endif

    std::ostringstream ss;
    ss << std::put_time(&tm, "%F %T");  // e.g., 2025-06-25 13:45:12
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();  // append .XYZ
    return ss.str();
}

//----------------------------------------------------------------

std::string
Logger::levelToString(Level level) const
{
    switch (level)
    {
        case Level::Debug: return "DEBUG";
        case Level::Trace: return "TRACE";
        case Level::Info:  return "INFO";
        case Level::Warn:  return "WARN";
        case Level::Error: return "ERROR";
    }
    return "UNKNOWN";
}

//----------------------------------------------------------------

rang::fg
Logger::levelColor(Level level) const
{
    using namespace rang;
    switch (level) {
        case Level::Debug: return fg::cyan;
        case Level::Trace: return fg::cyan;
        case Level::Info:  return fg::green;
        case Level::Warn:  return fg::yellow;
        case Level::Error: return fg::red;
    }
    return fg::gray;
}

//----------------------------------------------------------------

void
Logger::setDebugLevel(bool onOff)
{
    debugOn_ = onOff;
}

//----------------------------------------------------------------

void
Logger::setTraceLevel(bool onOff)
{
    traceOn_ = onOff;
}

//----------------------------------------------------------------

bool
Logger::isDebugOn() const
{
    return debugOn_;
}

//----------------------------------------------------------------

bool
Logger::isTraceOn() const
{
    return traceOn_;
}

//----------------------------------------------------------------
