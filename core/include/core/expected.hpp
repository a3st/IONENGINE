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
    ~Expected() {
        if(_has_value) {
            _data.ok.~Type();
        } else {
            _data.error.~Error();
        }
    }

    Expected(Expected const& other) {
        if(other._has_value) {
            _data.ok = other._data.ok;
        } else {
            _data.error = other._data.error;
        }
        _has_value = other.has_value;
    }

    Expected(Expected&& other) noexcept {
        if(other._has_value) {
            _data.ok = std::move(other._data.ok);
        } else {
            _data.error = std::move(other._data.error);
        }
        _has_value = other.has_value;
    }

    Expected& operator=(Expected const& other) {
        if(other._has_value) {
            _data.ok = other._data.ok;
        } else {
            _data.error = other._data.error;
        }
        _has_value = other.has_value;
        return *this;
    }

    Expected& operator=(Expected&& other) noexcept {
        if(other._has_value) {
            _data.ok = std::move(other._data.ok);
        } else {
            _data.error = std::move(other._data.error);
        }
        _has_value = other.has_value;
        return *this;
    }

    ///
    /// Check for error
    /// @return Result of expected
    ///
    inline bool has_value() const { return _has_value; }

    ///
    /// Get the first type
    /// @return First type
    ///
    inline Type ok() {
        assert(_has_value && "core::Expected has not value");
        return std::move(_data.ok); 
    }

    ///
    /// Get the second type
    /// @return Second type
    ///
    inline Error error() { 
        assert(!_has_value && "core::Expected has value");
        return std::move(_data.error); 
    }

 private:
    Expected(Type&& element) : _has_value(true) {
        new (&_data.ok) Type(std::move(element));
    }

    Expected(Error&& element) : _has_value(false) {
        new (&_data.error) Error(std::move(element));
    }

    union Data {
        Type ok;
        Error error;

        Data() { }
        ~Data() { }
    } _data;
    bool _has_value;
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
