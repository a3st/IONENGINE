// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <mpsc_channel/mpsc_channel.hpp>

namespace ionengine::lib {

template<class Type>
class EventDispatcher {
public:

    EventDispatcher() = default;

    void add(mpsc::Sender<Type> const& sender) {

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

    std::vector<mpsc::Sender<Type>> _senders;
};

}
