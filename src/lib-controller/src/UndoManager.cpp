//----------------------------------------------------------------
//
// File: UndoManager.cpp
//
//----------------------------------------------------------------

#include <controller/UndoManager.h>

using namespace Ctrl;

//----------------------------------------------------------------

void
UndoManager::push(std::unique_ptr<UndoBet> ubet)
{
    stack_.push_back(std::move(ubet));
}

//----------------------------------------------------------------

bool
UndoManager::canUndo() const
{
    return !stack_.empty();
}

//----------------------------------------------------------------

void
UndoManager::undo()
{
    if (stack_.empty()) return;
    auto pUbet = std::move(stack_.back());
    stack_.pop_back();
    pUbet->undo();
}

//----------------------------------------------------------------

void
UndoManager::clear()
{
    stack_.clear();
}

//----------------------------------------------------------------
