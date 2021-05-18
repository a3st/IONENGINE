// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/singleton.h"

namespace ionengine {

class Logger final : public Singleton<Logger> {
DECLARE_SINGLETON(Logger)
public:

protected:

    Logger() {
        
    }

private:
};

}