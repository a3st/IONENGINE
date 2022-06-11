// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <lib/event_dispatcher.h>

#pragma once

namespace ionengine::lib {

enum class LoggerCategoryType {
    Renderer,
    Engine,
    Ui,
    Exception
};

enum class LoggerMessageType {
    Log,
    Warning,
    Error
};

struct LoggerEvent {
    LoggerMessageType message_type;
    std::string message;
    LoggerCategoryType category;
    std::time_t time;
};

class Logger {
public:

    Logger();

    void log(LoggerCategoryType const category, std::string_view const message);

    void warning(LoggerCategoryType const category, std::string_view const message);

    void error(LoggerCategoryType const category, std::string_view const message);

    void throw_messages();

    void color_mode(bool const enable);

private:

    lib::EventDispatcher<LoggerEvent> _event_dispatcher;
    std::optional<lib::Receiver<LoggerEvent>> _event_receiver;

    std::string constexpr logger_category_type_to_string(LoggerCategoryType const category) const;

    bool _is_color_mode{true};

    std::string const _log_color_code{"\033[0m"};
    std::string const _warning_color_code{"\x1B[33m"};
    std::string const _error_color_code{"\x1B[31m"};
};

extern Logger _logger;

#ifndef INITIALIZE_LOGGER
#define INITIALIZE_LOGGER() lib::Logger lib::_logger;
#endif

inline Logger& logger() {
    return _logger;
}

}
