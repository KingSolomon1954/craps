//----------------------------------------------------------------
//
// File: MenuTable.h
//
//----------------------------------------------------------------

#pragma once

#include <cui/bases/MenuBase.h>
#include <cui/core/CuiStructs.h>

namespace Cui {

class MenuTable : public MenuBase
{
public:
    /// @name Lifecycle
    /// @{
   ~MenuTable() = default;
    static MenuTable& instance();
    /// @}

    /// @name Modifiers
    /// @{
    void draw()                          override;
    bool handleKey(int ch)               override;
    void setLocation(WindowPosition pos) override;
    /// @}

    /// @name Observers
    /// @{
    LocationRequest getLocationRequest() const override;
    /// @}

private:
    WindowPosition   winPos_{};
    const WindowSize winSize_ = {7, 24};  // rows, cols

private:
    MenuTable();  // Private ctor
    void fillWindow();
    void doListTables();
    void doSwitchToTable();
    void back();
};

/*-----------------------------------------------------------*//**

@class MenuTable

@brief The table menu, from Control, Tables

Responsibilities of MenuTable

@li Key bindings for the menu: list the tables, switch to another
@li Starts the table operations in OperationManager
@li Renders the menu on screen

*/

} // namespace Cui

//----------------------------------------------------------------
