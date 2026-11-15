//----------------------------------------------------------------
//
// File: Logger.h
//
//----------------------------------------------------------------

#pragma once

#include <fstream>
#include <mutex>
#include <string>
#include <vector>
#include <rang.hpp>

namespace Gen {

class Logger
{
public:
    static Logger& instance();
    
    void setOutputFile(const std::string& filename);
    bool hasOutputFile() const;
    void logDebug(const std::string& msg);
    void logTrace(const std::string& msg);
    void logInfo (const std::string& msg);
    void logWarn (const std::string& msg);
    void logError(const std::string& msg);

    void enableConsoleLogging();
    void disableConsoleLogging();
    
    void setDebugLevel(bool onOff);
    void setTraceLevel(bool onOff);

    bool isDebugOn() const;
    bool isTraceOn() const;
    
private:
    mutable std::mutex mutex_;
    bool debugOn_ = false;
    bool traceOn_ = false;

    enum class Level
    {
        Debug,
        Trace,
        Info,
        Warn,
        Error
    };

    // A message logged before there is an output file is formatted
    // right away (so its timestamp is true) and held here. The next
    // setOutputFile() writes them ahead of anything else.
    struct Pending
    {
        std::string line;       // Complete, ready to write
    };
    static constexpr size_t MaxPending = 1000;
    std::vector<Pending> pending_;

    std::ofstream out_;
    bool consoleEnabled_ = true;
    
    Logger() = default;
    void log(Level level, const std::string& message);
    std::string timestamp() const;
    std::string levelToString(Level level) const;
    rang::fg levelColor(Level level) const;
};

// Compile time toggling of debug and trace logging.
#ifdef DEBUG
#define LOG_DEBUG(msg) Gen::Logger::instance().logDebug(msg)
#define LOG_TRACE(msg) Gen::Logger::instance().logTrace(msg)
#else
#define LOG_DEBUG(msg) do {} while(0)
#define LOG_TRACE(msg) do {} while(0)
#endif

#define LOG_INFO(msg)  Gen::Logger::instance().logInfo (msg)
#define LOG_WARN(msg)  Gen::Logger::instance().logWarn (msg)
#define LOG_ERROR(msg) Gen::Logger::instance().logError(msg)
    
} // namespace Gen

//----------------------------------------------------------------
