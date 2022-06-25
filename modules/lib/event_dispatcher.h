// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/channel.h>

namespace ionengine::lib {

template<class Type>
class EventDispatcher {
public:

    EventDispatcher() = default;

    void add(Sender<Type> const& sender) {
        _senders.emplace_back(sender);
    }

    void broadcast(Type const& event) {
        for(auto& sender : _senders) {
            sender.send(event);
        }
    }

private:

    std::vector<Sender<Type>> _senders;
};

}
