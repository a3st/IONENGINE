// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::fg {

class Timeline {
public:

    Timeline(const TimelineType type) : m_type(type) {

    }

private:

    TimelineType m_type;

};

}