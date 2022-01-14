// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

template<class T>
class HandlePool {
public:

    HandlePool() {

        _elements.reserve(128);
        _spaces.reserve(128);
    }

    size_t push(T const& element) {

        if(_offset != _elements.size()) {
            auto it = std::find_if(
                _spaces.begin(), 
                _spaces.end(), 
                [&](auto& elem) { return elem == 0x0; }
            );

            if(it != _spaces.end()) {
                size_t index = std::distance(_spaces.begin(), it);
                _elements[index] = element;
                _spaces[index] = 0x1;
            } else {
                _offset = _elements.size();
                _elements.emplace_back(element);
                _spaces.emplace_back(0x1);
            }
        } else {
            _elements.emplace_back(element);
            _spaces.emplace_back(0x1);
        }
        return ++_offset;
    }

    void erase(size_t const index) {

        _spaces[index] = 0x0;
        _offset = index;
    }

    void clear() {
        
        std::memset(_spaces.data(), 0x0, sizeof(uint8_t) * _spaces.size());
        _offset = 0;
    }

    T& operator[](size_t const index) {
        
        return _elements[index];
    }

private:

    size_t _offset{0};
    std::vector<T> _elements;
    std::vector<uint8_t> _spaces;
};

}