// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/channel.h>

namespace ionengine::lib {

///
/// Event dispatcher allows you to send events to all senders
///
template <class Type>
class EventDispatcher {
 public:
    EventDispatcher() = default;

    ///
    /// Add a new sender for event mailings
    /// @param sender to whom events will be sent
    ///
    inline void add(Sender<Type> const& sender) { _senders.emplace_back(sender); }

    ///
    /// Broadcast message to all senders
    /// @param event object to be broadcasted
    ///
    void broadcast(Type const& event) {
        for (auto& sender : _senders) {
            sender.send(event);
        }
    }

 private:
    std::vector<Sender<Type>> _senders;
};

}  // namespace ionengine::lib
