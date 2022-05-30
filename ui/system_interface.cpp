// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <ui/system_interface.h>

using namespace ionengine;
using namespace ionengine::ui;

SystemInterface::SystemInterface(lib::Logger& logger) :
    _logger(&logger) {

}

SystemInterface::~SystemInterface() {

}

bool SystemInterface::LogMessage(Rml::Log::Type type, const Rml::String& message) {
	switch(type) {
        case Rml::Log::LT_INFO:
        case Rml::Log::LT_DEBUG:
        case Rml::Log::LT_ALWAYS: {
            _logger->log(lib::LoggerCategoryType::Ui, message);
        } break;
        case Rml::Log::LT_WARNING: {
            _logger->warning(lib::LoggerCategoryType::Ui, message);
        } break;
        case Rml::Log::LT_ASSERT:
        case Rml::Log::LT_ERROR: {
            _logger->error(lib::LoggerCategoryType::Ui, message);
        } break;
	}
    return true;
}

double SystemInterface::GetElapsedTime() {
    return 0.0;
}