//----------------------------------------------------------------
//
// File: ErrorPass.cpp
//
//----------------------------------------------------------------

#include <gen/ErrorPass.h>
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <sstream>

using namespace Gen;

using ErrorName = ErrorPass::ErrorName;

/*-----------------------------------------------------------*//**

Construct an ErrorPass object.

Initialized to empty, and timestamp set to epoch minimum.

*/
ErrorPass::ErrorPass()
    : severity_(Severity::UnsetSeverity)
    , lineNum_(0)
    , codeNum_(0)
    , ts_(std::chrono::system_clock::time_point::min())
{
}

//----------------------------------------------------------------

void
ErrorPass::setErrorName(ErrorPass::ErrorName n)
{
    if (n == ErrorPass::ErrorName::UnsetErrorName)
        errorName_.clear();
    else
        errorName_ = ErrorPass::errorNameStr(n);
}

//----------------------------------------------------------------

void
ErrorPass::setErrorName(const std::string& name)
{
    errorName_ = name;
}

//----------------------------------------------------------------

void
ErrorPass::setErrorType(ErrorPass::ErrorType n)
{
    if (n == ErrorPass::ErrorType::ErrorTypeNotSpecified)
        errorType_.clear();
    else
        errorType_ = ErrorPass::errorTypeStr(n);
}

//----------------------------------------------------------------

void
ErrorPass::setErrorType(const std::string& type)
{
    errorType_ = type;
}

//----------------------------------------------------------------

void
ErrorPass::setSeverity(ErrorPass::Severity s)
{
    severity_ = s;
}

//----------------------------------------------------------------

const std::string&
ErrorPass::errorName() const
{
    return errorName_;
}

//----------------------------------------------------------------

const std::string&
ErrorPass::errorNameStr() const
{
    return errorName_;
}

//----------------------------------------------------------------
//
// Internal static strings for lookup
//
namespace
{
    const std::string UnsetErrorNameStr            = "UnsetErrorName";
    const std::string NullPointerErrorStr          = "Null pointe";
    const std::string NullValueErrorStr            = "Null value";
    const std::string InvalidArgumentErrorStr      = "Invalid argument";
    const std::string RangeErrorStr                = "Out of range";
    const std::string TimeoutErrorStr              = "Timeout";
    const std::string LogicErrorStr                = "Logic error";
    const std::string IllegalStateErrorStr         = "Illegal state";
    const std::string InvalidAccessErrorStr        = "Invalid access";
    const std::string NoPermissionErrorStr         = "No permission";
    const std::string SignalErrorStr               = "SignalError";
    const std::string NotFoundErrorStr             = "Item not found";
    const std::string ExistsErrorStr               = "Item already exists";
    const std::string SystemCallErrorStr           = "System call error";
    const std::string LengthErrorStr               = "Length error";
    const std::string OverflowErrorStr             = "Overflow error";
    const std::string UnderflowErrorStr            = "Underflow error";
    const std::string DataFormatErrorStr           = "Data format error";
    const std::string InputOutputErrorStr          = "Input output error";
    const std::string NetworkErrorStr              = "Network error";
    const std::string ConfigurationErrorStr        = "Configuration error";
    const std::string CaughtExceptionStr           = "Caught exception";
    const std::string OperationNotPossibleErrorStr = "Operation not possible";
    const std::string NotImplementedErrorStr       = "Not Implemented";
}

const std::string&
ErrorPass::errorNameStr(const ErrorPass::ErrorName& en)
{
    switch (en)
    {
    case ErrorName::UnsetErrorName:            return UnsetErrorNameStr;
    case ErrorName::CaughtException:           return CaughtExceptionStr;
    case ErrorName::ConfigurationError:        return ConfigurationErrorStr;
    case ErrorName::DataFormatError:           return DataFormatErrorStr;
    case ErrorName::ExistsError:               return ExistsErrorStr;
    case ErrorName::IllegalStateError:         return IllegalStateErrorStr;
    case ErrorName::InputOutputError:          return InputOutputErrorStr;
    case ErrorName::InvalidAccessError:        return InvalidAccessErrorStr;
    case ErrorName::InvalidArgumentError:      return InvalidArgumentErrorStr;
    case ErrorName::LengthError:               return LengthErrorStr;
    case ErrorName::LogicError:                return LogicErrorStr;
    case ErrorName::NetworkError:              return NetworkErrorStr;
    case ErrorName::NoPermissionError:         return NoPermissionErrorStr;
    case ErrorName::NotFoundError:             return NotFoundErrorStr;
    case ErrorName::NotImplementedError:       return NotImplementedErrorStr;
    case ErrorName::NullPointerError:          return NullPointerErrorStr;
    case ErrorName::NullValueError:            return NullValueErrorStr;
    case ErrorName::OperationNotPossibleError: return OperationNotPossibleErrorStr;
    case ErrorName::OverflowError:             return OverflowErrorStr;
    case ErrorName::RangeError:                return RangeErrorStr;
    case ErrorName::SignalError:               return SignalErrorStr;
    case ErrorName::SystemCallError:           return SystemCallErrorStr;
    case ErrorName::TimeoutError:              return TimeoutErrorStr;
    case ErrorName::UnderflowError:            return UnderflowErrorStr;
    }
    return UnsetErrorNameStr;
}

//----------------------------------------------------------------

const std::string&
ErrorPass::errorType() const
{
    return errorType_;
}

//----------------------------------------------------------------

const std::string&
ErrorPass::errorTypeStr() const
{
    return errorType_;
}

//----------------------------------------------------------------

namespace
{
    const std::string ErrorTypeNotSpecifiedStr  = "ErrorTypeNotSpecified";
    const std::string CommunicationsErrorStr    = "Communications Error";
    const std::string EnvironmentalErrorStr     = "Environmental Error";
    const std::string EquipmentErrorStr         = "Equipment Error";
    const std::string ProcessingErrorStr        = "Processing Error";
    const std::string ProgrammingErrorStr       = "Programming Error";
    const std::string QualityOfServiceErrorStr  = "Quality of Service Error";
}

const std::string&
ErrorPass::errorTypeStr(const ErrorPass::ErrorType& et)
{
    switch (et)
    {
    case ErrorType::ErrorTypeNotSpecified: return ErrorTypeNotSpecifiedStr;
    case ErrorType::CommunicationsError:   return CommunicationsErrorStr;
    case ErrorType::ConfigurationError:    return ConfigurationErrorStr;
    case ErrorType::EnvironmentalError:    return EnvironmentalErrorStr;
    case ErrorType::EquipmentError:        return EquipmentErrorStr;
    case ErrorType::ProcessingError:       return ProcessingErrorStr;
    case ErrorType::ProgrammingError:      return ProgrammingErrorStr;
    case ErrorType::QualityOfServiceError: return QualityOfServiceErrorStr;
    }
    return ErrorTypeNotSpecifiedStr;
}

//----------------------------------------------------------------

ErrorPass::Severity
ErrorPass::severity() const
{
    return severity_;
}

//----------------------------------------------------------------

const std::string&
ErrorPass::severityStr() const
{
    return ErrorPass::severityStr(severity_);
}

//----------------------------------------------------------------

namespace
{
    const std::string UnsetSeverityStr = "UnsetSeverity";
    const std::string IndeterminateStr = "Indeterminate";
    const std::string CriticalStr      = "Critical";
    const std::string MajorStr         = "Major";
    const std::string MinorStr         = "Minor";
    const std::string WarningStr       = "Warning";
}

const std::string&
ErrorPass::severityStr(const ErrorPass::Severity& s)
{
    switch (s)
    {
    case Severity::UnsetSeverity: return UnsetSeverityStr;
    case Severity::Indeterminate: return IndeterminateStr;
    case Severity::Critical:      return CriticalStr;
    case Severity::Major:         return MajorStr;
    case Severity::Minor:         return MinorStr;
    case Severity::Warning:       return WarningStr;
    }
    return UnsetSeverityStr;
}

//----------------------------------------------------------------

void
ErrorPass::setDescription(const std::string& s)
{
    description_ = s;
}

//----------------------------------------------------------------

const std::string&
ErrorPass::description() const
{
    return description_;
}

//----------------------------------------------------------------

void
ErrorPass::setProbableCause(const std::string& s)
{
    probableCause_ = s;
}

//----------------------------------------------------------------

const std::string&
ErrorPass::probableCause() const
{
    return probableCause_;
}

//----------------------------------------------------------------

void
ErrorPass::setProposedRepair(const std::string& s)
{
    proposedRepair_ = s;
}

//----------------------------------------------------------------

const std::string&
ErrorPass::proposedRepair() const
{
    return proposedRepair_;
}

//----------------------------------------------------------------

void
ErrorPass::setAdditionalInfo(const std::string& s)
{
    additionalInfo_ = s;
}

//----------------------------------------------------------------

const std::string&
ErrorPass::additionalInfo() const
{
    return additionalInfo_;
}

//----------------------------------------------------------------

void
ErrorPass::setFileName(const std::string& s)
{
    fileName_ = s;
}

//----------------------------------------------------------------

const std::string&
ErrorPass::fileName() const
{
    return fileName_;
}

//----------------------------------------------------------------

void
ErrorPass::setFuncName(const std::string& s)
{
    funcName_ = s;
}

//----------------------------------------------------------------

const std::string&
ErrorPass::funcName() const
{
    return funcName_;
}

//----------------------------------------------------------------

void
ErrorPass::setLineNum(unsigned n)
{
    lineNum_ = n;
}

//----------------------------------------------------------------

unsigned
ErrorPass::lineNum() const
{
    return lineNum_;
}

//----------------------------------------------------------------

void
ErrorPass::setCodeNum(unsigned n)
{
    codeNum_ = n;
}

//----------------------------------------------------------------

unsigned
ErrorPass::codeNum() const
{
    return codeNum_;
}

/*-----------------------------------------------------------*//**

Set the time of this error. Defaults to now if unassigned.

*/
void
ErrorPass::setTimestamp(const std::chrono::system_clock::time_point& tp)
{
    if (tp == std::chrono::system_clock::time_point{})
    {
        ts_ = std::chrono::system_clock::now();
    }
    else
    {
        ts_ = tp;
    }
}

std::chrono::system_clock::time_point
ErrorPass::timestamp() const
{
    return ts_;
}

/*-----------------------------------------------------------*//**

Appends modern location tracking info to the call chain vector history.

*/
void
ErrorPass::pushTrace(const std::string& funcName, const std::string& fileName, unsigned lineNum)
{
    stackTrace_.push_back(fmt::format("{}() at {}:{}", funcName, fileName, lineNum));
}

//----------------------------------------------------------------

const std::vector<std::string>&
ErrorPass::stackTrace() const
{
    return stackTrace_;
}

//----------------------------------------------------------------

void
ErrorPass::prepend(const std::string& s)
{
    description_ = s + description_;
}

//----------------------------------------------------------------

void
ErrorPass::append(const std::string& s)
{
    description_ += s;
}

//----------------------------------------------------------------

void
ErrorPass::set(ErrorPass::ErrorName errorName,
               ErrorPass::ErrorType errorType,
               ErrorPass::Severity severity,
               const std::string& description,
               const std::string& funcName,
               const std::string& fileName,
               unsigned lineNum)
{
    const std::string name =
        errorName == ErrorPass::ErrorName::UnsetErrorName
            ? std::string{}
            : ErrorPass::errorNameStr(errorName);
    const std::string type =
        errorType == ErrorPass::ErrorType::ErrorTypeNotSpecified
            ? std::string{}
            : ErrorPass::errorTypeStr(errorType);

    set(name, type, severity, description, funcName, fileName, lineNum);
}

//----------------------------------------------------------------

void
ErrorPass::set(const std::string& errorName,
               const std::string& errorType,
               ErrorPass::Severity severity,
               const std::string& description,
               const std::string& funcName,
               const std::string& fileName,
               unsigned lineNum)
{
    errorName_   = errorName;
    errorType_   = errorType;
    severity_    = severity;
    description_ = description;
    funcName_    = funcName;
    fileName_    = fileName;
    lineNum_     = lineNum;
    ts_          = std::chrono::system_clock::now();

    // Seed initial point of failure into the stack trace
    stackTrace_.clear();
    pushTrace(funcName, fileName, lineNum);
}

//----------------------------------------------------------------

void
ErrorPass::clear()
{
    errorName_.clear();
    errorType_.clear();
    severity_  = Severity::UnsetSeverity;
    description_.clear();
    funcName_.clear();
    fileName_.clear();
    lineNum_ = 0;
    codeNum_ = 0;
    ts_ = std::chrono::system_clock::time_point::min();
    stackTrace_.clear();

}

//----------------------------------------------------------------

bool
ErrorPass::hasError() const
{
    return !errorName_.empty();
}

//----------------------------------------------------------------

ErrorPass::operator const std::string&() const
{
    return errorName_;
}

//----------------------------------------------------------------

bool
ErrorPass::operator== (const ErrorPass& rhs) const
{
    return
    errorName_   == rhs.errorName_   &&
    errorType_   == rhs.errorType_   &&
    severity_    == rhs.severity_    &&
    description_ == rhs.description_ &&
    funcName_    == rhs.funcName_    &&
    fileName_    == rhs.fileName_    &&
    lineNum_     == rhs.lineNum_     &&
    codeNum_     == rhs.codeNum_     &&
    ts_          == rhs.ts_          &&
    stackTrace_  == rhs.stackTrace_;
}

//----------------------------------------------------------------

std::string
ErrorPass::fullDesc() const
{
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

/*---------------------------------------------------------*//**

Output formatter utilizing {fmt} for speed and ISO8601 millisecond math.

*/

namespace Gen {

std::ostream&
operator<< (std::ostream& out, const ErrorPass& ep)
{
    // Extract millisecond remainder for complete ISO8601 formatting alignment
    const auto epoch_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
        ep.timestamp().time_since_epoch()).count();

    auto ms = epoch_ms % 1000;
    std::time_t time_t_ts = std::chrono::system_clock::to_time_t(ep.timestamp());
    std::tm tm_utc = *std::gmtime(&time_t_ts);
    std::string outStr = fmt::format(
        "\n"
        "          Error: {}\n"
        "           Type: {}\n"
        "       Severity: {}\n"
        "    Description: {}\n"
        "           Date: {:%Y-%m-%dT%H:%M:%S}.{:03}Z\n"
        " Probable Cause: {}\n"
        "Proposed Repair: {}\n"
        "Additional info: {}\n"
        "             At: {}:{}\n"
        "             In: {}()\n"
        "     Error code: {} (0 == unused)\n",
        ep.errorNameStr(),
        ep.errorTypeStr(),
        ep.severityStr(),
        ep.description(),
        tm_utc, ms,
        ep.probableCause(),
        ep.proposedRepair(),
        ep.additionalInfo(),
        ep.fileName(), ep.lineNum(),
        ep.funcName(),
        ep.codeNum()
        );

    if (!ep.stackTrace().empty())
    {
        outStr += "    Function Chain:\n";
        for (size_t i = 0; i < ep.stackTrace().size(); ++i)
        {
            outStr += fmt::format("      [{}] {}\n", i, ep.stackTrace()[i]);
        }
    }
    out << outStr;
    return out;
}

}  // namespace Gen

//----------------------------------------------------------------
