//----------------------------------------------------------------
//
// File: UndoManager.cpp
//
//----------------------------------------------------------------

#include <controller/UndoManager.h>
#include <gen/Logger.h>

using namespace Ctrl;

//----------------------------------------------------------------

UndoManager& UndoManager::instance()
{
    static UndoManager um;
    return um;
}

//----------------------------------------------------------------

void
UndoManager::push(const Craps::PlayerId& playerId,
                  std::unique_ptr<UndoBet> change,
                  bool joinPrevious)
{
    if (!change) return;

    std::lock_guard<std::mutex> lock(mutex_);
    Stack& stack = stacks_[playerId];

    if (joinPrevious && !stack.empty())
    {
        auto* group = dynamic_cast<UndoGroup*>(stack.back().get());
        if (group == nullptr)
        {
            // The previous step becomes the first of a group
            auto newGroup = std::make_unique<UndoGroup>();
            newGroup->add(std::move(stack.back()));
            group = newGroup.get();
            stack.back() = std::move(newGroup);
        }
        group->add(std::move(change));
        return;
    }
    stack.push_back(std::move(change));
}

//----------------------------------------------------------------

Gen::ReturnCode
UndoManager::undo(const Craps::PlayerId& playerId,
                  std::string& description,
                  Gen::ErrorPass& ep)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = stacks_.find(playerId);
    if (it == stacks_.end() || it->second.empty())
    {
        EP_SET(ep, Gen::ErrorPass::ErrorName::NotFoundError,
                   Gen::ErrorPass::ErrorType::ProcessingError,
                   Gen::ErrorPass::Severity::Indeterminate,
               "There is nothing to undo. Only bets made or changed since "
               "the last roll can be undone.");
        ep.setErrorName("Nothing to Undo");
        return Gen::ReturnCode::Fail;
    }

    Stack& stack = it->second;
    description = stack.back()->describe();

    if (stack.back()->undo(ep) == Gen::ReturnCode::Fail)
    {
        LOG_ERROR("UndoManager::undo(): unable to undo \"" + description +
                  "\"; dropping undo history." + ep.fullDesc());
        stacks_.erase(it);
        ep.prepend("Unable to undo: " + description + ". ");
        return Gen::ReturnCode::Fail;
    }

    stack.pop_back();
    if (stack.empty()) stacks_.erase(it);
    return Gen::ReturnCode::Success;
}

//----------------------------------------------------------------

bool
UndoManager::canUndo(const Craps::PlayerId& playerId) const
{
    return depth(playerId) > 0;
}

//----------------------------------------------------------------

std::size_t
UndoManager::depth(const Craps::PlayerId& playerId) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = stacks_.find(playerId);
    return (it == stacks_.end()) ? 0 : it->second.size();
}

//----------------------------------------------------------------

void
UndoManager::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    stacks_.clear();
}

//----------------------------------------------------------------

void
UndoManager::clear(const Craps::PlayerId& playerId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    stacks_.erase(playerId);
}

//----------------------------------------------------------------
