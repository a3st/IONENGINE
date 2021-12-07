// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform { class Window; }

namespace ionengine::renderer {

class Backend {
public:

    Backend(platform::Window* window);

    ~Backend();

    Backend(Backend const&) = delete;

    Backend(Backend&&) = delete;

    Backend& operator=(Backend const&) = delete;

    Backend& operator=(Backend&&) = delete;

private:

    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}