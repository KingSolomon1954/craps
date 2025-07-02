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
    void setOutputFile(const std::string& filename);
    void logDebug(const std::string& msg);
    void logInfo (const std::string& msg);
    void logWarn (const std::string& msg);
    void logError(const std::string& msg);

private:
    enum class Level
    {
        Debug,
        Info,
        Warn,
        Error
    };

    std::ofstream out_;

    void log(Level level, const std::string& message);
    std::string timestamp() const;
    std::string levelToString(Level level) const;
    rang::fg levelColor(Level level) const;
};

} // namespace Gen

//----------------------------------------------------------------

