//----------------------------------------------------------------
//
// File: EventManager.h
//
//----------------------------------------------------------------

#pragma once

#include <unordered_map>
#include <vector>
#include <functional>
#include <typeindex>
#include <any>

namespace App {

class EventManager
{
public:
    template<typename EventType>
    void subscribe(std::function<void(const EventType&)> handler)
    {
        auto wrapper = [handler](const std::any& e)
        {
            handler(std::any_cast<const EventType&>(e));
        };
        subscribers_[typeid(EventType)].push_back(wrapper);
    }

    template<typename EventType>
    void publish(const EventType& evt) const
    {
        auto it = subscribers_.find(typeid(EventType));
        if (it != subscribers_.end())
        {
            for (const auto& func : it->second)
            {
                func(evt);
            }
        }
    }

private:
    using AnyHandler = std::function<void(const std::any&)>;
    std::unordered_map<std::type_index, std::vector<AnyHandler>> subscribers_;
};

/*-----------------------------------------------------------*//**

@class EventManager

@brief Announce events

Pub/Sub various events.

*/

} // namespace App

//----------------------------------------------------------------

