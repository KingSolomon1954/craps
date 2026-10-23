//---------------------------------------------------------------
//
// File: StringUtils.cpp
//
//---------------------------------------------------------------

#include "gen/StringUtils.h"  // Our interface
#include <algorithm>          // std::equal
#include <sstream>            // For stringstream
#include <stdio.h>            // for vsnprintf

using namespace Gen;
using std::string;

const std::string StringUtils::whiteSpace(" \t\v\f\r\n");

/*-----------------------------------------------------------*//**

Returns a std::string formated with printf style arguments.

@param[in] pFmt
    The format specification.

@return
    Formatted string.
*/
std::string
StringUtils::toString(const char* pFmt, ...)
{
    va_list args;
    va_start(args, pFmt);

    char buf[1024];

    unsigned n = vsnprintf(buf, sizeof(buf), pFmt, args);
    va_end(args);

    if (n < sizeof(buf))
    {
        return std::string(buf);
    }

    va_start(args, pFmt);

    // Buffer not big enough.
    char* pBuf = new char[n + 1];
    vsnprintf(pBuf, n + 1, pFmt, args);
    va_end(args);

    std::string s(pBuf);
    delete[] pBuf;
    return s;
}

/*-----------------------------------------------------------*//**

Determine if a string starts with the given string.

@param[in] s1
    The string to operate on.

@param[in] s2
    The string to look for.

@return
    True if s1 starts with s2, otherwise false.
*/
bool
StringUtils::startsWith(const std::string& s1, const std::string& s2)
{
    if (s2.empty())
    {
        return false;
    }
    return s1.find(s2) == 0;
}

/*-----------------------------------------------------------*//**

Determine if a string starts with any of the characters in a 
given string.

@param[in] s1
    The string to operate on.

@param[in] any
    The characters to look for as a std::string.

@return
    True if s1 starts with any characters in any, otherwise false.
*/
bool
StringUtils::startsWithAnyOf(const std::string& s1, const std::string& any)
{
    return s1.find_first_not_of(any) != 0;
}

/*-----------------------------------------------------------*//**

Determine if the string ends with the given string.

@param[in] s1
    The string to operate on.

@param[in] s2
    The string to look for.

@return
    True if s1 ends with s2, otherwise false.
*/
bool
StringUtils::endsWith(const std::string& s1, const std::string& s2)
{
    string::size_type pos = s1.rfind(s2);
    return (pos + s2.length()) == s1.length();
}

/*-----------------------------------------------------------*//**

Determine if a string ends with any of the characters in a 
given string.

@param[in] s1
    The string to operate on.

@param[in] any
    The characters to look for as a std::string.

@return
    True if s1 ends with any of the characters in any, otherwise false.
*/
bool
StringUtils::endsWithAnyOf(const std::string& s1, const std::string& any)
{
    return s1.find_last_not_of(any) != s1.length() - 1;;
}

/*-----------------------------------------------------------*//**

Determine if a string contains any characters in the given string.

@param[in] s1
    The string to operate on.

@param[in] any
    The characters to look for in s.

@return
    True if s1 contains and characters in any, otherwise false.
*/
bool
StringUtils::containsAnyOf(const std::string& s1, const std::string& any)
{
    return s1.find_first_of(any) != string::npos;
}

/*-----------------------------------------------------------*//**

Removes the substring s2 from the back of string s1 in-place.

@param[in,out] s1
    The string to operate on.

@param[in] s2
    The substring to remove.
*/
void
StringUtils::trimEnd(std::string& s1, const std::string& s2)
{
    string::size_type endpos = s1.find_last_not_of(s2);
    s1.erase(endpos + 1);
}

/*-----------------------------------------------------------*//**

Removes the substring s2 from the front of string s1 in-place.

@param[in,out] s1
    The string to operate on.

@param[in] s2
    The substring to remove.
*/
void
StringUtils::trimFront(std::string& s1, const std::string& s2)
{
    string::size_type startpos = s1.find_first_not_of(s2);
    if (startpos != 0)
    {
        s1.erase(0, startpos);
    }
}

/*-----------------------------------------------------------*//**

Removes the substring s2 from the front and back of string s1 in-place.

@param[in,out] s1
    The string to operate on.

@param[in] s2
    The substring to remove.
*/
void
StringUtils::trim(std::string& s1, const std::string& s2)
{
    trimFront(s1, s2);
    trimEnd(s1, s2);
}

/*-----------------------------------------------------------*//**

Removes the C-style substring from the front and back of 
string s1 in-place.

@param[in,out] s1
    The string to operate on.

@param[in] pChars
    The C-style substring to remove.
*/
void
StringUtils::trim(std::string& s1, const char* pChars)
{
    string s2(pChars);
    trimFront(s1, s2);
    trimEnd(s1, s2);
}

/*-----------------------------------------------------------*//**

Removes the given character from the front and back of string
in-place.

@param[in,out] s1
    The string to operate on.

@param[in] c
    The character to remove.
*/
void
StringUtils::trim(std::string& s1, char c)
{
    string s2(1, c);
    trimFront(s1, s2);
    trimEnd(s1, s2);
}

/*-----------------------------------------------------------*//**

Replace all occurences of one substring with another.

@param[in,out] s1
    The string to operate on.

@param[in] substringOld
    The substring within s that needs replacing.

@param[in] substringNew
    The replacement substring.
*/
void
StringUtils::replace(std::string& s1,
                     const std::string& substringOld,
                     const std::string& substringNew)
{
    string::size_type pos;
    while ((pos = s1.find(substringOld)) != string::npos)
    {
        s1.erase(pos, substringOld.length());
        s1.insert(pos, substringNew);
    }
}

/*-----------------------------------------------------------*//**

Return the total number of occurrences of c in s1.

@param[in] s1
    The string to operate on.

@param[in] c
    The character to count.

@return
    The total number of occurrences of c in s1.
*/
int
StringUtils::countCharInString(const std::string& s1, char c)
{
    std::string::const_iterator i;
    int rv = 0;

    for (i = s1.begin(); i != s1.end(); i++)
    {
        if (*i == c)
        {
            rv++;
        }
    }

    return rv;
}

/*-----------------------------------------------------------*//**

Predicate for insensitive comparison of string objects.

@param[in] s1
    The first character to use for comparison.

@param[in] s2
    The second character to use for comparison.

@return
    True if the characters have the same case-insensitive
    content, otherwise false.
*/
bool
StringUtils::insensitivePredicate(
    const std::string::value_type &s1,
    const std::string::value_type &s2)
{
    return toupper(s1) == toupper(s2);
}

/*-----------------------------------------------------------*//**

Case insensitive comparison of two string objects.

@param[in] s1
    The first string to use for comparison.

@param[in] s2
    The second string to use for comparison.

@return
    True, if the strings have the same case-insensitive
    content, otherwise false.
*/
bool
StringUtils::insensitiveEquals(const std::string& s1, const std::string& s2)
{
    return s1.size() == s2.size() && std::equal(
        s1.begin(),
        s1.end(),
        s2.begin(),
        insensitivePredicate);
}

/*-----------------------------------------------------------*//**

Returns a new string converted to upper case.

@param[in] s1
    The string to convert.

@return
    The converted string.
*/
std::string
StringUtils::toUpper(const std::string& s1)
{
    std::string result("");
    for (char c : s1)
    { 
        result += std::toupper(c); 
    } 
    return result; 
}

/*-----------------------------------------------------------*//**

Returns a new string converted to lower case.

@param[in] s1
    The string to convert.

@return
    The converted string.
*/
std::string
StringUtils::toLower(const std::string& s1)
{
    std::string result(""); 
    for (char c : s1)
    { 
        result += std::tolower(c); 
    } 
    return result; 
}

/*-----------------------------------------------------------*//**

Wrap one explicit input line into lines no wider than maxWidth.

Leading and trailing spaces and tabs are removed. Words are kept
together where possible; a word longer than maxWidth is hard-wrapped.

@param[in]  line     The input line, without its line terminator.
@param[in]  maxWidth Maximum output line width; must be at least 1.
@param[out] result   Receives the wrapped line or lines. An empty
                     input line produces one empty output line.
*/
void
StringUtils::wrapLine(const std::string& line,
                      size_t maxWidth,
                      std::vector<std::string>& result)
{
    const auto first = line.find_first_not_of(" \t");

    if (first == std::string::npos)
    {
        result.emplace_back();
        return;
    }

    const auto last = line.find_last_not_of(" \t");
    const std::string trimmed = line.substr(first, last - first + 1);

    size_t pos = 0;

    while (pos < trimmed.size())
    {
        // Skip whitespace before the next word.
        while (pos < trimmed.size() &&
               (trimmed[pos] == ' ' || trimmed[pos] == '\t'))
        {
            ++pos;
        }

        if (pos >= trimmed.size())
            break;

        const size_t remaining = trimmed.size() - pos;

        if (remaining <= maxWidth)
        {
            result.push_back(trimmed.substr(pos));
            break;
        }

        // Search backwards for whitespace within the allowed width.
        size_t breakPos = pos + maxWidth;

        while (breakPos > pos &&
               trimmed[breakPos] != ' ' &&
               trimmed[breakPos] != '\t')
        {
            --breakPos;
        }

        if (breakPos == pos)
        {
            // The next word exceeds maxWidth; hard-wrap it.
            result.push_back(trimmed.substr(pos, maxWidth));
            pos += maxWidth;
        }
        else
        {
            result.push_back(trimmed.substr(pos, breakPos - pos));
            pos = breakPos;
        }
    }
}

/*-----------------------------------------------------------*//**

Wrap a vector of possibly formatted strings into a single vector
of output lines.

Each input string is wrapped independently using
wrapFormattedString(). Explicit line breaks and blank lines within each
input string are preserved. The resulting lines are appended in the same
order as the input strings.

@note One design detail: this treats each vector element as an
independent formatted string. It does not merge adjacent elements into
one paragraph, so each element's leading/trailing whitespace handling
and explicit line breaks follow the behavior of wrapFormattedString().

@param[in]  strings  The input strings to wrap.
@param[in]  maxWidth Maximum number of characters per output line;
                     values less than 1 are treated as 1.
@param[out] result   Receives all wrapped lines, in input order.
                     Existing contents are cleared before processing.
*/
void
StringUtils::wrapFormattedStrings(
    const std::vector<std::string>& strings,
    int maxWidth,
    std::vector<std::string>& result)
{
    result.clear();

    std::vector<std::string> wrapped;

    for (const auto& s : strings)
    {
        StringUtils::wrapFormattedString(s, maxWidth, wrapped);

        result.insert(result.end(),
                      wrapped.begin(),
                      wrapped.end());
    }
}

/*-----------------------------------------------------------*//**
  
Wrap a formatted string into lines of a specified maximum width.

Existing line breaks are preserved, including blank lines. CRLF,
CR, and LF are recognized as line endings. Lines are trimmed of
leading and trailing spaces and tabs, and long lines are wrapped
at whitespace where possible. Words longer than maxWidth are
hard-wrapped.

Some important details:

@li Empty input: Returns an empty vector.
@li Blank lines: Preserved, including consecutive line breaks.
@li Trailing newline: Preserves the final empty line. For example,
    "Hello\n" produces {"Hello", ""}.
@li Long words: Hard-wrapped to respect the width limit.
@li Whitespace: Leading and trailing spaces and tabs are stripped
    from each explicit input line; wrapping whitespace is not retained.
@li Width: A value below 1 is treated as 1.

One subtle point: this wraps by bytes/characters in std::string, not
terminal display columns. That's appropriate for ordinary ASCII text,
but UTF-8 characters and wide terminal glyphs may occupy multiple bytes
or columns.

@param[in]  s        The input string to wrap.
@param[in]  maxWidth Maximum number of characters per output line;
                     values less than 1 are treated as 1.
@param[out] result   Receives the wrapped lines. Existing contents
                     are cleared before the result is generated.
*/
void
StringUtils::wrapFormattedString(const std::string& s,
                                 int maxWidth,
                                 std::vector<std::string>& result)
{
    result.clear();

    const size_t width = static_cast<size_t>(std::max(1, maxWidth));

    size_t start = 0;

    while (start <= s.size())
    {
        const size_t end = s.find_first_of("\r\n", start);
        const size_t len = (end == std::string::npos)
                         ? s.size() - start
                         : end - start;

        wrapLine(s.substr(start, len), width, result);

        if (end == std::string::npos)
            break;

        // Consume the line ending, treating CRLF as a single newline.
        start = end + 1;

        if (s[end] == '\r' &&
            start < s.size() &&
            s[start] == '\n')
        {
            ++start;
        }
    }
}

//----------------------------------------------------------------

std::vector<std::string>
StringUtils::wrapText(const std::string& text, int wrapWidth)
{
    std::vector<std::string> lines;
    std::istringstream iss(text);
    std::string word, line;

    while (iss >> word)
    {
        if (line.empty())
        {
            line = word;
        }
        else if ((int)line.size() + 1 + (int)word.size() <= wrapWidth)
        {
            line += " " + word;
        }
        else
        {
            lines.push_back(line);
            line = word;
        }
    }
    if (!line.empty())
    {
        lines.push_back(line);
    }
    return lines;
}

//----------------------------------------------------------------
