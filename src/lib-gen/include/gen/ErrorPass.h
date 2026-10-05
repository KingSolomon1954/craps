//----------------------------------------------------------------
//
// File: ErrorPass.h
//
//----------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <ostream>
#include <cstdint>
#include <chrono>

namespace Gen {

//---------------------------------------------------------------

class ErrorPass
{
public:
    /**
     * @brief Standard error names used with Error Passing.
     *
     * These generally agree with the standard exception classes.
     */
    enum class ErrorName : std::uint8_t
    {
        UnsetErrorName,                 ///< No error
        CaughtException,                ///<
        ConfigurationError,             ///<
        DataFormatError,                ///<
        ExistsError,                    ///<
        IllegalStateError,              ///<
        InputOutputError,               ///<
        InvalidAccessError,             ///<
        InvalidArgumentError,           ///<
        LengthError,                    ///<
        LogicError,                     ///<
        NetworkError,                   ///<
        NoPermissionError,              ///<
        NotFoundError,                  ///<
        NotImplementedError,            ///<
        NullPointerError,               ///<
        NullValueError,                 ///<
        OperationNotPossibleError,      ///<
        OverflowError,                  ///<
        RangeError,                     ///<
        SignalError,                    ///<
        SystemCallError,                ///<
        TimeoutError,                   ///<
        UnderflowError,                 ///<
    };

    /// Error Type
    enum class ErrorType : std::uint8_t
    {
        ErrorTypeNotSpecified,          ///<
        CommunicationsError,            ///<
        ConfigurationError,             ///<
        EnvironmentalError,             ///<
        EquipmentError,                 ///<
        ProcessingError,                ///<
        QualityOfServiceError           ///<
    };

    /// Severity
    enum class Severity : std::uint8_t
    {
        UnsetSeverity,                  ///<
        Indeterminate,                  ///<
        Critical,                       ///<
        Major,                          ///<
        Minor,                          ///<
        Warning                         ///<
    };

    /// @name Lifecycle
    /// @{
    ErrorPass();
   ~ErrorPass() = default;
    /// @}

    /// @name Modifiers
    /// @{
    void set(ErrorPass::ErrorName errorName,
             ErrorPass::ErrorType errorType,
             ErrorPass::Severity  severity,
             const std::string&   description,
             const std::string&   funcName,
             const std::string&   fileName,
             unsigned             lineNum);
    void set(const std::string&  errorName,
             const std::string&  errorType,
             ErrorPass::Severity severity,
             const std::string&  description,
             const std::string&  funcName,
             const std::string&  fileName,
             unsigned            lineNum);
    void clear();
    void prepend(const std::string& s);
    void append(const std::string& s);
    void setErrorName(ErrorPass::ErrorName errorName);
    void setErrorName(const std::string& errorName);
    void setErrorType(ErrorPass::ErrorType errorType);
    void setErrorType(const std::string& errorType);
    void setSeverity(ErrorPass::Severity severity);
    void setDescription(const std::string& s);
    void setProbableCause(const std::string& s);
    void setProposedRepair(const std::string& s);
    void setAdditionalInfo(const std::string& s);
    void setFileName(const std::string& s);
    void setFuncName(const std::string& s);
    void setLineNum(unsigned n);
    void setCodeNum(unsigned n);
    void setTimestamp(const std::chrono::system_clock::time_point& tp = std::chrono::system_clock::time_point{});
    void pushTrace(const std::string& funcName, const std::string& fileName, unsigned lineNum);
    /// @}

    /// @name Observers
    /// @{
    operator const std::string&()         const;
    const std::string& errorName()         const;
    const std::string& errorType()         const;
    ErrorPass::Severity severity()         const;
    const std::string& errorNameStr()      const;
    const std::string& errorTypeStr()      const;
    const std::string& severityStr()      const;
    const std::string& fileName()         const;
    const std::string& funcName()         const;
    unsigned lineNum()                    const;
    unsigned codeNum()                    const;
    std::string fullDesc()                const;
    const std::string& description()      const;
    const std::string& probableCause()    const;
    const std::string& proposedRepair()   const;
    const std::string& additionalInfo()   const;
    std::chrono::system_clock::time_point timestamp() const;
    const std::vector<std::string>& stackTrace() const;
    bool hasError()                       const;
    bool operator== (const ErrorPass& ep) const;
    /// @}

    static const std::string& errorNameStr(const ErrorPass::ErrorName& en);
    static const std::string& errorTypeStr(const ErrorPass::ErrorType& et);
    static const std::string& severityStr (const ErrorPass::Severity& s);

    friend std::ostream& operator<< (std::ostream& out, const ErrorPass& ep);

private:
    std::string errorName_;
    std::string errorType_;
    Severity severity_;
    std::string description_;
    std::string probableCause_;
    std::string proposedRepair_;
    std::string additionalInfo_;
    std::string funcName_;
    std::string fileName_;
    unsigned lineNum_;
    unsigned codeNum_;
    std::chrono::system_clock::time_point ts_;
    std::vector<std::string> stackTrace_;
};

/*-----------------------------------------------------------*//**

@class ErrorPass

@brief A generic error payload for value-based error handling.

Error passing combined with `zeus::expected` provides a type-safe,
value-based error handling scheme. Instead of throwing exceptions or using
primitive return status codes, functions return a `zeus::expected<T, ErrorPass>`.

If a function succeeds, it contains its value of type `T`. If it fails, it
carries an `ErrorPass` payload containing metadata about the failure and
the location where it occurred.

The standard ErrorName and ErrorType enums provide names for common library
errors. Applications may also supply their own error names and types as
strings; no derived error class or enum extension is required. Error names
and types are stored as strings. An empty error name means that no error is
set. Severity remains a standard enum.

Example using the standard vocabulary:

@code
ErrorPass ep;
EP_SET(ep, ErrorPass::ErrorName::NetworkError,
       ErrorPass::ErrorType::CommunicationsError,
       ErrorPass::Severity::Critical,
       "Failed socket write");
@endcode

Example using application-defined vocabulary:

@code
ErrorPass ep;
EP_SET(ep, "Insufficient Funds", "Betting Error",
       ErrorPass::Severity::Warning,
       "Player does not have sufficient funds for this wager");
@endcode

When a parent function receives an error from a child, it can append its
own context to the stack trace before propagating the error, preserving
the failure path without manual side-channel handling, as shown
in the example below.

Example Usage:
@code
using Gen::ErrorPass;

zeus::expected<int, ErrorPass> Socket::write(char *pData, int len)
{
    if (write(sockFd_, pData, len) < 0)
    {
        ErrorPass ep;
        EP_SET(ep, ErrorPass::ErrorName::NetworkError,
            ErrorPass::ErrorType::CommunicationsError,
            ErrorPass::Severity::Critical,
            "Failed socket write: " + std::string(strerror(errno)));
        return zeus::unexpected(ep);
    }
    return len;
}

// Embellishing errors up the call chain:
auto result = socket.write(buf, len);
if (!result)
{
    auto ep = result.error();
    EP_TRACE(ep); // Automatically appends caller information to stackTrace vector
    return zeus::unexpected(ep);
}
@endcode

@ingroup error
*/

} // namespace Gen

/*-----------------------------------------------------------*//**

@def EP_SET

@brief Convenience macro to populate a newly generated error passing object.

@ingroup error
*/
#define EP_SET(epObj, errName, errType, severity, errDescription)        \
    epObj.set(errName, errType, severity, errDescription, __FUNCTION__, __FILE__, __LINE__)

/*-----------------------------------------------------------*//**

@def EP_TRACE

@brief Appends the current call stack context to the ErrorPass payload.

@ingroup error
*/
#define EP_TRACE(epObj) \
    epObj.pushTrace(__FUNCTION__, __FILE__, __LINE__)

//----------------------------------------------------------------
