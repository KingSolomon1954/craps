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
    const std::string NullPointerErrorStr          = "NullPointerError";
    const std::string NullValueErrorStr            = "NullValueError";
    const std::string InvalidArgumentErrorStr      = "InvalidArgumentError";
    const std::string RangeErrorStr                = "RangeError";
    const std::string TimeoutErrorStr              = "TimeoutError";
    const std::string LogicErrorStr                = "LogicError";
    const std::string IllegalStateErrorStr         = "IllegalStateError";
    const std::string InvalidAccessErrorStr        = "InvalidAccessError";
    const std::string NoPermissionErrorStr         = "NoPermissionError";
    const std::string SignalErrorStr               = "SignalError";
    const std::string NotFoundErrorStr             = "NotFoundError";
    const std::string ExistsErrorStr               = "ExistsError";
    const std::string SystemCallErrorStr           = "SystemCallError";
    const std::string LengthErrorStr               = "LengthError";
    const std::string OverflowErrorStr             = "OverflowError";
    const std::string UnderflowErrorStr            = "UnderflowError";
    const std::string DataFormatErrorStr           = "DataFormatError";
    const std::string InputOutputErrorStr          = "InputOutputError";
    const std::string NetworkErrorStr              = "NetworkError";
    const std::string ConfigurationErrorStr        = "ConfigurationError";
    const std::string CaughtExceptionStr           = "CaughtException";
    const std::string OperationNotPossibleErrorStr = "OperationNotPossibleError";
    const std::string NotImplementedErrorStr       = "NotImplementedError";
}

const std::string&
ErrorPass::errorNameStr(const ErrorPass::ErrorName& en)
{
    switch (en)
    {
    case ErrorName::UnsetErrorName:            return UnsetErrorNameStr;
    case ErrorName::NullPointerError:          return NullPointerErrorStr;
    case ErrorName::NullValueError:            return NullValueErrorStr;
    case ErrorName::InvalidArgumentError:      return InvalidArgumentErrorStr;
    case ErrorName::RangeError:                return RangeErrorStr;
    case ErrorName::TimeoutError:              return TimeoutErrorStr;
    case ErrorName::LogicError:                return LogicErrorStr;
    case ErrorName::IllegalStateError:         return IllegalStateErrorStr;
    case ErrorName::InvalidAccessError:        return InvalidAccessErrorStr;
    case ErrorName::NoPermissionError:         return NoPermissionErrorStr;
    case ErrorName::SignalError:               return SignalErrorStr;
    case ErrorName::NotFoundError:             return NotFoundErrorStr;
    case ErrorName::ExistsError:               return ExistsErrorStr;
    case ErrorName::SystemCallError:           return SystemCallErrorStr;
    case ErrorName::LengthError:               return LengthErrorStr;
    case ErrorName::OverflowError:             return OverflowErrorStr;
    case ErrorName::UnderflowError:            return UnderflowErrorStr;
    case ErrorName::DataFormatError:           return DataFormatErrorStr;
    case ErrorName::InputOutputError:          return InputOutputErrorStr;
    case ErrorName::NetworkError:              return NetworkErrorStr;
    case ErrorName::ConfigurationError:        return ConfigurationErrorStr;
    case ErrorName::CaughtException:           return CaughtExceptionStr;
    case ErrorName::OperationNotPossibleError: return OperationNotPossibleErrorStr;
    case ErrorName::NotImplementedError:       return NotImplementedErrorStr;
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
    const std::string CommunicationsErrorStr    = "CommunicationsError";
    const std::string EnvironmentalErrorStr     = "EnvironmentalError";
    const std::string EquipmentErrorStr         = "EquipmentError";
    const std::string ProcessingErrorStr        = "ProcessingError";
    const std::string QualityOfServiceErrorStr  = "QualityOfServiceError";
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
