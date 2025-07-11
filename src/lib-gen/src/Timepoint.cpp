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
    fromString(iso8601);
}

//----------------------------------------------------------------
//
// Assignment from string
//
Timepoint&
Timepoint::operator=(const std::string& iso8601)
{
    this->fromString(iso8601);
    return *this;
}

//----------------------------------------------------------------
//
// Common conversion from string
//
void
Timepoint::fromString(const std::string& iso8601)
{
    std::istringstream iss(iso8601);
    iss >> std::chrono::parse("%Y-%m-%dT%H:%M:%S", tp_);
    if (iss.fail())
    {
        std::string diag = std::format("Invalid ISO8601 format: \"{}\"", iso8601);
        throw std::invalid_argument(diag);
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

#if 0

#include <chrono>
#include <string>
#include <sstream>
#include <regex>
#include <stdexcept>

std::chrono::system_clock::duration parseDurationWithDays(const std::string& input) {
    using namespace std::chrono;

    std::regex re(R"(^\s*(\d+)d\s+(\d{2}:\d{2}:\d{2})\s*$)");
    std::smatch match;

    if (!std::regex_match(input, match, re) || match.size() != 3) {
        throw std::invalid_argument("Invalid format. Expected 'Nd HH:MM:SS'");
    }

    int daysCount = std::stoi(match[1].str());
    std::string timePart = match[2].str();

    // Parse the time portion
    std::istringstream timeStream(timePart);
    std::chrono::hours h;
    std::chrono::minutes m;
    std::chrono::seconds s;
    std::chrono::time_point<system_clock, seconds> tp;

    timeStream >> std::chrono::parse("%T", tp);
    if (timeStream.fail()) {
        throw std::runtime_error("Failed to parse time portion");
    }

    // Return combined duration
    return days(daysCount) + tp.time_since_epoch();
}


int main() {
    using namespace std::chrono;

    auto dur = parseDurationWithDays("2d 03:01:55");
    std::cout << "Total seconds: " << duration_cast<seconds>(dur).count() << '\n';  // 183715
}


#include <chrono>
#include <format>
#include <string>

std::string formatDurationWithDays(std::chrono::system_clock::duration dur) {
    using namespace std::chrono;

    // Extract whole days and remainder
    auto totalSeconds = duration_cast<seconds>(dur);
    auto daysPart = duration_cast<days>(totalSeconds);
    auto timePart = totalSeconds - daysPart;

    // Convert remainder to a time_point so we can format it
    auto tp = time_point<system_clock, seconds>(timePart);

    return std::format("{}d {:%T}", daysPart.count(), tp);  // %T = HH:MM:SS
}


#include <iostream>

int main() {
    using namespace std::chrono;

    auto d = days(2) + hours(3) + minutes(1) + seconds(55);
    std::string out = formatDurationWithDays(d);
    std::cout << out << "\n";  // Output: "2d 03:01:55"
}


#endif
