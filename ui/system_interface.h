// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <RmlUi/Core/SystemInterface.h>
#include <lib/logger.h>

namespace ionengine::ui {

class SystemInterface : public Rml::SystemInterface {
public:

    SystemInterface(lib::Logger& logger);

    ~SystemInterface();

    virtual double GetElapsedTime() override;

    virtual bool LogMessage(Rml::Log::Type type, const Rml::String& message) override;

private:

    lib::Logger* _logger;

};

}