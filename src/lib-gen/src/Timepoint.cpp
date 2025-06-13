//----------------------------------------------------------------
//
// File: Timepoint.cpp
//
//----------------------------------------------------------------

#include <gen/Timepoint.h>

#include <format>
#include <sstream>
#include <iomanip>
#include <stdexcept>

using namespace Gen;

//----------------------------------------------------------------
//
// Default constructor: now
//
Timepoint::Timepoint()
    : tp_(Clock::now())
{
    // empty
}

//----------------------------------------------------------------
//
// Construct from time_point
//
Timepoint::Timepoint(TimePoint tp)
    : tp_(tp)
{
    // empty
}

//----------------------------------------------------------------
//
// Construct from duration offset from now
//
Timepoint::Timepoint(Duration offsetFromNow)
    : tp_(Clock::now() + offsetFromNow)
{
    // empty
}

//----------------------------------------------------------------
//
// Construct from ISO8601 string
//
Timepoint::Timepoint(const std::string& iso8601)
{
    std::istringstream iss(iso8601);
    iss >> std::chrono::parse("%Y-%m-%dT%H:%M:%S", tp_);
    if (iss.fail())
    {
        throw std::invalid_argument("Invalid ISO8601 format: " + iso8601);
    }
}

//----------------------------------------------------------------
//
// Convert to ISO8601 string
//
std::string
Timepoint::toString() const
{
    return std::format("{:%Y-%m-%dT%H:%M:%S}", tp_);
}

//----------------------------------------------------------------

Timepoint::operator std::string() const
{
    return toString();
}

//----------------------------------------------------------------
//
// Duration between two Timepoints
//
Timepoint::Duration
Timepoint::operator-(const Timepoint& other) const
{
    return tp_ - other.tp_;
}

//----------------------------------------------------------------
//
// Duration between Timepoint and now
//
Timepoint::Duration
Timepoint::sinceNow() const
{
    return tp_ - Clock::now();
}

//----------------------------------------------------------------

// Comparison operators
bool Timepoint::operator< (const Timepoint& other) const { return tp_ <  other.tp_; }
bool Timepoint::operator> (const Timepoint& other) const { return tp_ >  other.tp_; }
bool Timepoint::operator<=(const Timepoint& other) const { return tp_ <= other.tp_; }
bool Timepoint::operator>=(const Timepoint& other) const { return tp_ >= other.tp_; }
bool Timepoint::operator==(const Timepoint& other) const { return tp_ == other.tp_; }
bool Timepoint::operator!=(const Timepoint& other) const { return tp_ != other.tp_; }

//----------------------------------------------------------------
//
// True if Timepoint is earlier than Clock::now()
//
bool
Timepoint::hasExpired() const
{
    return tp_ < Clock::now();
}

//----------------------------------------------------------------
//
// True if Timepoint is later than Clock::now()
//
bool
Timepoint::isFuture() const
{
    return tp_ > Clock::now();
}

//----------------------------------------------------------------
//
// Accessor
//
Timepoint::TimePoint Timepoint::raw() const
{
    return tp_;
}

//----------------------------------------------------------------
//
// Set to given time point.
// 
void
Timepoint::setTo(TimePoint tp)
{
    tp_ = tp;
}

//----------------------------------------------------------------
//
// Set to now.
// 
void
Timepoint::setToNow()
{
    tp_ = Clock::now();
}

//----------------------------------------------------------------
//
// Set to now plus an offset.
// 
void
Timepoint::setToNow(Duration offsetFromNow)
{
    tp_ = Clock::now() + offsetFromNow;
}

//----------------------------------------------------------------
//
// Stream output operator
//
std::ostream&
operator<< (std::ostream& os, const Gen::Timepoint& tp)
{
    return os << tp.toString();
}

//----------------------------------------------------------------

