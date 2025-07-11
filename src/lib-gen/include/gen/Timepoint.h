//----------------------------------------------------------------
//
// File: Timepoint.cpp
//
//----------------------------------------------------------------

#pragma once

#include <chrono>
#include <ostream>
#include <string>

namespace Gen {

class Timepoint
{
public:
    using Clock     = std::chrono::system_clock;
    using TimePoint = Clock::time_point;
    using Duration  = Clock::duration;

    // Constructors
    Timepoint();                                      // now
    explicit Timepoint(TimePoint tp);                 // from chrono time_point
    Timepoint(Duration offsetFromNow);                // now + duration
    explicit Timepoint(const std::string& iso8601);   // from string, throws on bad format

    // Assignment
    Timepoint& operator=(const std::string& str);
    Timepoint& operator=(const Timepoint& other) = default;
    
    // Resets
    void setTo(TimePoint tp);
    void setToNow();
    void setToNow(Duration offsetFromNow);
    
    // Conversion
    std::string toString() const;
    operator std::string() const;

    // Duration operations
    Duration operator-(const Timepoint& other) const;
    Duration sinceNow() const;

    // Comparisons
    bool operator< (const Timepoint& other) const;
    bool operator> (const Timepoint& other) const;
    bool operator<=(const Timepoint& other) const;
    bool operator>=(const Timepoint& other) const;
    bool operator==(const Timepoint& other) const;
    bool operator!=(const Timepoint& other) const;

    // Now-based logic
    bool hasExpired() const;
    bool isFuture() const;

    // Accessor
    TimePoint raw() const;

private:
    TimePoint tp_;
    
    void fromString(const std::string& iso8601);
};

/*-----------------------------------------------------------*//**

@class Timepoint

@brief Timepoint abstraction with various conversions and operators

Layers on top of std::chrono::system_clock::time_point.

*/

} // namespace Gen

std::ostream& operator<< (std::ostream& os, const Gen::Timepoint& tp);

//----------------------------------------------------------------
