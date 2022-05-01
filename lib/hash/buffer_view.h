// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib::hash {

template<class Type>
class BufferView {
public:

    BufferView(std::span<Type const> const data) {

        _data = data;
        _data_hash = std::hash<std::string_view>()(std::string_view(reinterpret_cast<char const*>(_data.data()), reinterpret_cast<char const*>(_data.data() + _data.size())));
    }

    uint64_t hash() const {
        
        return _data_hash;
    }

    Type const* data() const {

        return _data.data();
    }

    size_t size() const {

        return _data.size();
    }

private:
    
    std::span<Type const> _data;
    uint64_t _data_hash{0};
    uint32_t _count{0};
};

}
