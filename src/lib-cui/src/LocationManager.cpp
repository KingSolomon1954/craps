//----------------------------------------------------------------
//
// File: LocationManager.cpp
//
//----------------------------------------------------------------

#include <cui/LocationManager.h>
#include <cui/layouts/LayoutConsole.h>
#include <cassert>

using namespace Cui;

//----------------------------------------------------------------

LocationManager()
{
    screenRows_ = LayoutConsole::height;
    screenCols_ = LayoutConsole::width;
}

/*-----------------------------------------------------------*//**

@brief Finds and reserves the best screen position for a surface.

The resulting position is registered under surfaceName so that
subsequent placement requests consider the new surface occupied.

@return std::nullopt if no suitable position is available, otherwise
        the position the surface should be place at.

*/
std::optional<WindowPosition>
LocationManager::findPosition(const Request& request,
                              const std::string& parentName,
                              const std::string& surfaceName) const
{
    std::optonal<WindowPosition> position;
    
    if (request.kind == PlacementKind::Dialog)
    {
        position = findDialogPosition(request);
    }
    else if (!parentName.empty())
    {
        auto parentRect = getRec(parentName);
        if (parentRect)
            position = findMenuPosition(request, parentRect);
    }
    else
    {
        // Independent popup
        position = findIndependentPosition(request);
    }

    if (!position) return std::nullopt;
    
    WindowRect rect{
        position->row, position->col,
        request.size.rows, request.size.cols};
    
    windows_[surfaceName] = rect;
    return position;
}

//----------------------------------------------------------------

std::optional<WindowPosition>
LocationManager::findDialogPosition(const Request& request) const
{
    const int row = (screenRows_ - size.rows) / 2;
    const int col = (screenCols_ - size.cols) / 2;

    WindowRect rect{row, col, size.rows, size.cols};

    if (fitsOnScreen(rect) && !isOccupied(rect))
    {
        return WindowPosition{row, col};
    }
}

//----------------------------------------------------------------

std::optional<WindowPosition>
LocationManager::findMenuPosition(const Request& request,
                                  const std::string& parentName) const
{
    if (!parentName.empty())
    {
        const auto& rect = getRectByName(parentName);

        // Preferred: right of parent.
        if (request.direction == Direction::Right)
        {
            WindowRect candidate{rect.row, rect.right() + 1,
                                 size.rows,  size.cols};

            if (fitsOnScreen(candidate) && !isOccupied(candidate))
                return WindowPosition{candidate.row, candidate.col};
        }

        // Fallback: left of parent.
        {
            WindowRect candidate{rect.row, rect.col - size.cols,
                                 size.rows,  size.cols};

            if (fitsOnScreen(candidate) && !isOccupied(candidate))
                return WindowPosition{candidate.row, candidate.col};
        }

        // TODO Add more attempts to find a position
    }

    // Do we need generic fallback here?
    return std::nullopt;
}

//----------------------------------------------------------------

std::optional<WindowPosition>
LocationManager::findIndependentPosition(const Request& request) const
{
    const int row = (screenRows_ - size.rows) / 2;
    const int col = (screenCols_ - size.cols) / 2;

    WindowRect candidate{row, col, size.rows, size.cols};

    if (fitsOnScreen(candidate) && !isOccupied(candidate))
        return WindowPosition{row, col};

    return std::nullopt;
}

//----------------------------------------------------------------

bool
LocationManager::fitsOnScreen(const WindowRect& rect) const
{
    // TODO
}

//----------------------------------------------------------------

bool
LocationManager::overlapsExisting(const WindowRect& rect) const
{
    // TODO
}

//----------------------------------------------------------------

void
LocationManager::reserve(std::string id, WindowRect rect)
{
    // TODO
}

//----------------------------------------------------------------

void
LocationManager::release(const std::string& surfaceName)
{
    // TODO
}

//----------------------------------------------------------------
