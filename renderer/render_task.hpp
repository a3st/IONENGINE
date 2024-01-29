// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "cache/primitive_cache.hpp"
#include "render_queue.hpp"
#include "math/matrix.hpp"

namespace ionengine {

namespace renderer {

struct RenderTaskData {
    PrimitiveData primitive;
    uint32_t index_count;
    math::Matrixf model;
    uint8_t mask;
};

struct RenderTask {
    Primitive drawable;
    uint32_t index_count;
    math::Matrixf model;
    uint8_t mask;
};

template<typename Type>
class RenderTaskIterator {
public:

    using iterator_category = std::forward_iterator_tag;
    using value_type = Type;
    using difference_type = std::ptrdiff_t;
    using pointer = Type*;
    using reference = Type&;

    RenderTaskIterator() : ptr(nullptr) { }

    RenderTaskIterator(pointer ptr) : ptr(ptr) { }

    reference operator*() const { return *ptr; }

    pointer operator->() { return ptr; }

    auto operator++() -> RenderTaskIterator& {

        ptr++; 
        return *this; 
    }  

    auto operator++(int) -> RenderTaskIterator {

        RenderTaskIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    friend auto operator==(RenderTaskIterator const& lhs, RenderTaskIterator const& rhs) -> bool { 
            
        return lhs.ptr == rhs.ptr; 
    }

    friend auto operator!=(RenderTaskIterator const& lhs, RenderTaskIterator const& rhs) -> bool { 
            
        return lhs.ptr != rhs.ptr; 
    }

private:

    pointer ptr;
};

class RenderTaskStream {
public:

    using iterator = RenderTaskIterator<RenderTask>;
    using const_iterator = RenderTaskIterator<RenderTask const>;

    RenderTaskStream(PrimitiveCache& primitive_cache) : primitive_cache(&primitive_cache) {

    }

    auto begin() -> iterator {
        
        return tasks.data();
    }

    auto end() -> iterator {
        
        return tasks.data() + tasks.size();
    }

    auto cbegin() const -> const_iterator {

        return tasks.data();
    }

    auto cend() const -> const_iterator {

        return tasks.data() + tasks.size();
    }

    auto write(RenderTaskData const& data) -> void {

        auto result = primitive_cache->get(data.primitive);
        if(result) {
            auto render_task = RenderTask {
                .drawable = result.value(),
                .index_count = data.index_count,
                .model = data.model,
                .mask = data.mask
            };
            tasks.emplace_back(render_task);
        }
    }

    auto flush() -> void {

        tasks.clear();
    }

    friend auto operator<<(RenderTaskStream& lhs, const RenderTaskData& rhs) -> RenderTaskStream& {

        lhs.write(rhs);
        return lhs;
    }

private:

    PrimitiveCache* primitive_cache;
    std::vector<RenderTask> tasks;
};

}

}