//----------------------------------------------------------------
//
// File: Timepoint.cpp
//
//----------------------------------------------------------------

#include <gen/Timepoint.h>

#include <format>
#include <sstream>
#include <iomanip>
#include <regex>
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
    return Clock::now() - tp_;
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

/*-----------------------------------------------------------*//**

Parse a custom string like "2d 03:01:55" into a duration

@param[in] input
    A formatted string in the form of "2d 03:01:55".
    Optional leading zero(s) accepted e.g. "02d 03:01:55".

@return
    a chrono Duration 

@throws
    std::invalid_argument if expected time format is bad.
    
@throws
    std::runtime_error if passes regex but time format is still bad.
    
@internal

General approach:

* Split the days portion from the time string.
* Parse both parts manually or with std::chrono::parse.

Regex Breakdown:

* \s* — optional whitespace at start
* (\d+) — 1 or more digits for the days (✅ accepts 0, 09, 23, 999, etc.)
* d — literal 'd' character
* \s+ — at least one space between d and the time
* (\d{2}:\d{2}:\d{2}) — fixed 2-digit HH:MM:SS time format
* \s* — optional whitespace at end

Example usage:
@code
    auto dur = parseDurationWithDays("2d 03:01:55");
    std::cout << "Total seconds: " << duration_cast<seconds>(dur).count() << '\n';  // 183715
@endcode
*/
std::chrono::system_clock::duration
Timepoint::parseDurationWithDays(const std::string& input)
{
    using namespace std::chrono;

    std::regex re(R"(^\s*(\d+)d\s+(\d{2}:\d{2}:\d{2})\s*$)");
    std::smatch match;

    if (!std::regex_match(input, match, re) || match.size() != 3)
    {
        std::string diag("Timepoint::parseDurationWithDays() Invalid "
                         "format. Expected 'Nd HH:MM:SS' got ");
        throw std::invalid_argument(diag + input);
    }

    int daysCount = std::stoi(match[1].str());
    std::string timePart = match[2].str();

    std::istringstream timeStream(timePart);
    std::chrono::seconds hms{};  // parse into duration
    timeStream >> std::chrono::parse("%T", hms);
    if (timeStream.fail())
    {
        std::string diag("Timepoint::parseDurationWithDays() Invalid "
                         "format. Expected 'HH:MM:SS' got ");
        throw std::runtime_error(diag + timePart);
    }

    return days(daysCount) + hms;  // Return combined duration
}

/*-----------------------------------------------------------*//**

Turns a chrono duration into a string formatted as "Nd HH:MM:SS"

Example output: "2d 03:01:55"

@param[in] dur
    Duration

@return
    Formatted string

@internal

* %T formats as HH:MM:SS.
* Instead could have used std::format("{:%H:%M:%S}", tp) to be explicit.
* Assumes the duration is non-negative.

Example usage:
@code
    auto d = days(2) + hours(3) + minutes(1) + seconds(55);
    std::string out = formatDurationWithDays(d);
    std::cout << out << "\n";  // Output: "2d 03:01:55"
@endcode
*/
std::string
Timepoint::formatDurationWithDays(std::chrono::system_clock::duration dur)
{
    using namespace std::chrono;

    // Extract whole days and remainder
    auto totalSeconds = duration_cast<seconds>(dur);
    auto daysPart     = duration_cast<days>(totalSeconds);
    auto timePart     = totalSeconds - daysPart;

    // Convert remainder to a time_point so we can format it
    auto tp = time_point<system_clock, seconds>(timePart);

    return std::format("{}d {:%T}", daysPart.count(), tp);  // %T = HH:MM:SS
}

//----------------------------------------------------------------
