//----------------------------------------------------------------
//
// File: LocationManager.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/CuiStructs.h>
#include <optional>
#include <string>
#include <unordered_map>

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
        LocationKind kind = LocationKind::Menu;
        Direction direction = Direction::Right;
    };
    
    LocationManager();

    std::optional<WindowPosition>
    findPosition(const Request& request,
                 const std::string& parentName,
                 const std::string& surfaceName);

    void release(const std::string& surfaceName);

private:
    std::optional<WindowPosition>
    findMenuPosition(const Request& request,
                     const WindowRect& parentRect) const;

    std::optional<WindowPosition>
    findDialogPosition(const Request& request) const;

    std::optional<WindowPosition>
    findIndependentPosition(const Request& request) const;

    std::optional<WindowRect>
    getRect(const std::string& surfaceName) const;

    bool fitsOnScreen    (const WindowRect& rect) const;
    bool overlapsExisting(const WindowRect& rect) const;

private:
    int screenRows_;
    int screenCols_;
    std::unordered_map<std::string, WindowRect> windows_;
};

/*-----------------------------------------------------------*//**

@class LocationManager

@brief Decides where a new window rectangle should go.

Using already registered rectangles, LocationManager
decides where a new rectangle should go.

@li knows screen dimensions
@li knows rectangles of active surfaces
@li finds best position
@li reserves successful position
@li releases position when surface disappears

*/
    
}  // namespace Cui

//----------------------------------------------------------------
