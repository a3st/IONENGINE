// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <lib/logger.h>

using namespace ionengine;
using namespace ionengine::lib;

Logger::Logger() {
    auto [sender, receiver] = lib::make_channel<LoggerEvent>();
    _event_dispatcher.add(sender);
    _event_receiver.emplace(receiver);
}

void Logger::log(LoggerCategoryType const category, std::string_view const message) {
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    auto logger_event = LoggerEvent {
        .message_type = LoggerMessageType::Log,
        .message = std::string(message.begin(), message.end()),
        .category = category,
        .time = time
    };

    _event_dispatcher.broadcast(logger_event);
}

void Logger::warning(LoggerCategoryType const category, std::string_view const message) {
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    auto logger_event = LoggerEvent {
        .message_type = LoggerMessageType::Warning,
        .message = std::string(message.begin(), message.end()),
        .category = category,
        .time = time
    };

    _event_dispatcher.broadcast(logger_event);
}

void Logger::error(LoggerCategoryType const category, std::string_view const message) {
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    auto logger_event = LoggerEvent {
        .message_type = LoggerMessageType::Error,
        .message = std::string(message.begin(), message.end()),
        .category = category,
        .time = time
    };

    _event_dispatcher.broadcast(logger_event);
}

void Logger::throw_messages() {
    auto logger_event = LoggerEvent {};
    while(_event_receiver.value().try_receive(logger_event)) {
        
        char conv_time[32];
        auto time = tm {};

        errno_t result = localtime_s(&time, &logger_event.time);
        if(result != 0) {
            throw std::system_error(result, std::generic_category());
        }

        std::strftime(conv_time, sizeof(conv_time), "%H:%M:%S", &time);

        switch(logger_event.message_type) {
            case LoggerMessageType::Log: {
                if(_is_color_mode) {
                    std::cout << 
                        std::format("{} {} LOG: [{}] {} {}", 
                            _log_color_code, 
                            conv_time, 
                            logger_category_type_to_string(logger_event.category), 
                            logger_event.message, 
                            _log_color_code
                        ) 
                    << std::endl;
                } else {
                    std::cout << 
                        std::format("{} LOG: [{}] {}", 
                            conv_time, 
                            logger_category_type_to_string(logger_event.category), 
                            logger_event.message
                        )
                    << std::endl;
                }
            } break;
            case LoggerMessageType::Warning: {
                if(_is_color_mode) {
                    std::cout << 
                        std::format("{} {} WARN: [{}] {} {}", 
                            _warning_color_code, 
                            conv_time, 
                            logger_category_type_to_string(logger_event.category), 
                            logger_event.message, 
                            _log_color_code
                        )
                    << std::endl;
                } else {
                    std::cout << 
                        std::format("{} WARN: [{}] {}", 
                            conv_time, 
                            logger_category_type_to_string(logger_event.category), 
                            logger_event.message
                        )
                    << std::endl;
                }
            } break;
            case LoggerMessageType::Error: {
                if(_is_color_mode) {
                    std::cerr << 
                        std::format("{} {} ERR: [{}] {} {}", 
                            _error_color_code, 
                            conv_time, 
                            logger_category_type_to_string(logger_event.category), 
                            logger_event.message, 
                            _log_color_code
                        )
                    << std::endl;
                } else {
                    std::cerr << 
                        std::format("{} ERR: [{}] {}", 
                            conv_time, 
                            logger_category_type_to_string(logger_event.category), 
                            logger_event.message
                        )
                    << std::endl;
                }
            } break;
        }
    }
}

void Logger::color_mode(bool const enable) {
    _is_color_mode = enable;
}

std::string constexpr Logger::logger_category_type_to_string(LoggerCategoryType const category) const {
    switch(category) {
        case LoggerCategoryType::Engine: return "Engine";
        case LoggerCategoryType::Renderer: return "Renderer";
        case LoggerCategoryType::Exception: return "Exception";
        default: return "Unknown";
    }
}
