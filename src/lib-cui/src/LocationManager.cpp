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

LocationManager::LocationManager()
    : screenRows_(LayoutConsole::height),
      screenCols_(LayoutConsole::width)
{
}

/*-----------------------------------------------------------*//**

@brief Finds and reserves the best screen position for a surface.

The resulting position is registered under surfaceName so that
subsequent placement requests consider the new surface occupied.

@return std::nullopt if no suitable position is available, otherwise
        the position the surface should be placed at.

*/
std::optional<WindowPosition>
LocationManager::findPosition(const LocationRequest& request,
                              const std::string& parentName,
                              const std::string& surfaceName)
{
    std::optional<WindowPosition> position;
    assert(request.size.rows > 0 && request.size.cols > 0);

    if (request.kind == LocationKind::Dialog)
    {
        position = findDialogPosition(request);
    }
    else if (!parentName.empty())
    {
        const auto parentRect = getRect(parentName);

        if (parentRect)
        {
            position = findMenuPosition(request, *parentRect);
        }
    }
    else
    {
        // Independent popup.
        position = findIndependentPosition(request);
    }

    if (!position)
        return std::nullopt;

    const WindowRect rect{
        position->row,
        position->col,
        request.size.rows,
        request.size.cols
    };

    // A surface name should identify one active window. If this
    // surface is already registered, replace its old location.
    windows_[surfaceName] = rect;

    return position;
}

//----------------------------------------------------------------

std::optional<WindowPosition>
LocationManager::findDialogPosition(const LocationRequest& request) const
{
    const int row = (screenRows_ - request.size.rows) / 2;
    const int col = (screenCols_ - request.size.cols) / 2;

    const WindowRect rect{
        row, col,
        request.size.rows,
        request.size.cols
    };

    if (fitsOnScreen(rect) && !overlapsExisting(rect))
    {
        return WindowPosition{row, col};
    }

    return std::nullopt;
}

//----------------------------------------------------------------

std::optional<WindowPosition>
LocationManager::findMenuPosition(const LocationRequest& request,
                                  const WindowRect& parentRect) const
{
    // Preferred direction.
    if (request.direction == Direction::Right)
    {
        const WindowRect candidate{
            parentRect.row,
            parentRect.right() + 1,
            request.size.rows,
            request.size.cols
        };

        if (fitsOnScreen(candidate) && !overlapsExisting(candidate))
        {
            return WindowPosition{
                candidate.row,
                candidate.col
            };
        }
    }

    if (request.direction == Direction::Left)
    {
        const WindowRect candidate{
            parentRect.row,
            parentRect.col - request.size.cols,
            request.size.rows,
            request.size.cols
        };

        if (fitsOnScreen(candidate) && !overlapsExisting(candidate))
        {
            return WindowPosition{
                candidate.row,
                candidate.col
            };
        }
    }

    // Try the opposite horizontal direction.
    if (request.direction == Direction::Right)
    {
        const WindowRect candidate{
            parentRect.row,
            parentRect.col - request.size.cols,
            request.size.rows,
            request.size.cols
        };

        if (fitsOnScreen(candidate) && !overlapsExisting(candidate))
        {
            return WindowPosition{
                candidate.row,
                candidate.col
            };
        }
    }
    else
    {
        const WindowRect candidate{
            parentRect.row,
            parentRect.right() + 1,
            request.size.rows,
            request.size.cols
        };

        if (fitsOnScreen(candidate) && !overlapsExisting(candidate))
        {
            return WindowPosition{
                candidate.row,
                candidate.col
            };
        }
    }

    // TODO: additional placement strategies.

    return std::nullopt;
}

//----------------------------------------------------------------

std::optional<WindowPosition>
LocationManager::findIndependentPosition(const LocationRequest& request) const
{
    const int row = (screenRows_ - request.size.rows) / 2;
    const int col = (screenCols_ - request.size.cols) / 2;

    const WindowRect candidate{
        row,
        col,
        request.size.rows,
        request.size.cols
    };

    if (fitsOnScreen(candidate) && !overlapsExisting(candidate))
    {
        return WindowPosition{row, col};
    }

    return std::nullopt;
}

//----------------------------------------------------------------

bool
LocationManager::fitsOnScreen(const WindowRect& rect) const
{
    if (rect.row < 0 || rect.col < 0)
        return false;

    if (rect.rows <= 0 || rect.cols <= 0)
        return false;

    if (rect.bottom() >= screenRows_)
        return false;

    if (rect.right() >= screenCols_)
        return false;

    return true;
}

//----------------------------------------------------------------

bool
LocationManager::overlapsExisting(const WindowRect& rect) const
{
    for (const auto& [surfaceName, existing] : windows_)
    {
        const bool separated =
            rect.right()      < existing.col ||
            existing.right()  < rect.col     ||
            rect.bottom()     < existing.row ||
            existing.bottom() < rect.row;

        if (!separated)
            return true;
    }

    return false;
}

//----------------------------------------------------------------

std::optional<WindowRect>
LocationManager::getRect(const std::string& surfaceName) const
{
    const auto it = windows_.find(surfaceName);

    if (it == windows_.end())
        return std::nullopt;

    return it->second;
}

//----------------------------------------------------------------

void
LocationManager::release(const std::string& surfaceName)
{
    windows_.erase(surfaceName);
}

//----------------------------------------------------------------
