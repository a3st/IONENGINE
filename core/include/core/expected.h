// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core {

///
/// Expected class
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
    /// Check for error
    /// @return Result of expected
    ///
    inline bool has_error() const { return _data.index() == 1; }

    ///
    /// Get the first type
    /// @return First type
    ///
    inline Type ok() { return std::move(std::get<Type>(_data)); }

    ///
    /// Get the second type
    /// @return Second type
    ///
    inline Error error() { return std::move(std::get<Error>(_data)); }

 private:
    Expected(std::variant<Type, Error>&& element) : _data(std::move(element)) {}

    std::variant<Type, Error> _data;
};

///
/// Create an Error Handler with specified types
/// @param element Success element
///
template <class Type, class Error>
inline Expected<Type, Error> make_expected(Type&& element) {
    return Expected<Type, Error>(std::move(element));
}

///
/// Create an Error Handler with specified types
/// @param element Error element
///
template <class Type, class Error>
inline Expected<Type, Error> make_expected(Error&& element) {
    return Expected<Type, Error>(std::move(element));
}

}  // namespace ionengine::lib
