//----------------------------------------------------------------
//
// File: LocationManager.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/CuiStructs.h>
#include <unordered_map>
#include <string>

namespace Cui
{

class LocationManager
{
public:
    enum class Direction
    {
        Right,
        Left,
        Below,
        Above
    };

    enum class LocationKind
    {
        Menu,
        Dialog,
        Overlay,
        Panel,
        FullScreen
    };

    struct LocationInfo
    {
        int          height;
        int          width;
        LocationKind kind;
        Direction    direction;
    };

    struct Request
    {
        WindowSize size;
        LocationKind kind = LocationKind::Popup;
    };
    
    LocationManager();

    std::optional<WindowPosition>
    findPosition(const Request& request,
                 const std::string& parentName,
                 const std::string& surfaceName) const;

    void release(const std::string& surfaceName);

private:
    std::optional<WindowPosition>
    findMenuPosition(const Request& request,
                     const std::string& parentName) const;

    std::optional<WindowPosition>
    findDialogPosition(const Request& request) const;

    std::optional<WindowPosition>
    findIndependentPosition(const Request& request) const;

    void reserve(std::string id, WindowRect rect);
    
    bool fitsOnScreen    (const WindowRect& rect) const;
    bool overlapsExisting(const WindowRect& rect) const;

private:
    int screenRows_;
    int screenCols_;
    std::unordered_map<std::string, WindowRect> windows_;
};

/*-----------------------------------------------------------*//**

@class LocationManager

@brief todo

@li todo

*/
    
}  // namespace Cui

//----------------------------------------------------------------
