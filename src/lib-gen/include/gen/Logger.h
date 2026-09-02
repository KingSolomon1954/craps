//----------------------------------------------------------------
//
// File: Logger.h
//
//----------------------------------------------------------------

#pragma once

#include <fstream>
#include <string>
#include <rang.hpp>

namespace Gen {

class Logger
{
public:
    static Logger& instance();
    
    void setOutputFile(const std::string& filename);
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
#define LOG_DEBUG(msg) Gen::Logger::instance().logDebug(msg);
#define LOG_TRACE(msg) Gen::Logger::instance().logTrace(msg);
#else
#define LOG_DEBUG(msg) do {} while(0)
#define LOG_TRACE(msg) do {} while(0)
#endif
    
} // namespace Gen

//----------------------------------------------------------------
