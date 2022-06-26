// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

///
/// A common structure for all errors that accepts an enumeration of errors
///
template <class ErrorCode>
struct Result {
    ErrorCode errc;
    std::string message;
};

///
/// An error handler that throws the first type on success and the second type on error
///
template <class Type, class Error>
class Expected {
    friend Expected make_expected<Type, Error>(Type&& element);

    friend Expected make_expected<Type, Error>(Error&& element);

 public:
    Expected(Expected const&) = default;

    Expected(Expected&&) noexcept = default;

    Expected& operator=(Expected const&) = default;

    Expected& operator=(Expected&&) noexcept = default;

    ///
    /// Check for success
    /// @return Result of expected
    ///
    inline bool is_ok() const { return _data.index() == 0; }

    ///
    /// Check for error
    /// @return Result of expected
    ///
    inline bool is_error() const { return _data.index() == 1; }

    ///
    /// Getting the first type
    /// @return First type
    ///
    inline Type ok() { return std::move(std::get<Type>(_data)); }

    ///
    /// Getting the second type
    /// @return Second type
    ///
    inline Error error() { return std::move(std::get<Error>(_data)); }

 private:
    Expected(std::variant<Type, Error>&& element) : _data(std::move(element)) {}

    std::variant<Type, Error> _data;
};

///
/// Creating an Error Handler with specified types
/// @param element Element to be getting on success
///
template <class Type, class Error>
inline Expected<Type, Error> make_expected(Type&& element) {
    return Expected<Type, Error>(std::move(element));
}

///
/// Creating an Error Handler with specified types
/// @param element Element to be getting on error
///
template <class Type, class Error>
inline Expected<Type, Error> make_expected(Error&& element) {
    return Expected<Type, Error>(std::move(element));
}

}  // namespace ionengine::lib
