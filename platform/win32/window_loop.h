// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "winapi.h"

#include <common/platform/window_loop.h>

namespace ionengine::platform {

class WindowLoop : IWindowLoop {
public:

    WindowLoop();

    void run(const std::function<void(WindowEvent const&)>& run_func) override;
    
    void quit() override;

    void set_event(WindowEvent const& event);

private:

    WindowEvent event_;
    bool quit_;
};

}