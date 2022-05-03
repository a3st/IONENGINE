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

        std::for_each(
            _senders.begin(),
            _senders.end(),
            [&](auto& element) {

                element.send(event);
            }
        );
    }

private:

    std::vector<Sender<Type>> _senders;
};

}
