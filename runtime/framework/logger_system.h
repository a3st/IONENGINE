// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

class LoggerSystem {
public:

    LoggerSystem() {
        
    }

private:

};

std::unique_ptr<LoggerSystem> create_unique_logger_system() {
    return std::make_unique<LoggerSystem>();
}

}