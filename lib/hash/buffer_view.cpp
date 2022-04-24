// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <lib/hash/buffer_view.h>

using namespace ionengine::lib::hash;

BufferView::BufferView(std::span<uint8_t const> const data)  {
    
    _data = data;
    _data_hash = std::hash<std::string_view>()(std::string_view(reinterpret_cast<char const*>(_data.data()), reinterpret_cast<char const*>(_data.data() + _data.size())));
}

uint64_t BufferView::hash() const  {
    
    return _data_hash;
}

uint8_t const* BufferView::data() const  {
    
    return _data.data();
}

size_t BufferView::size() const  {
    
    return _data.size();
}
