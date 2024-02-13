// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "math/matrix.hpp"
#include "primitive.hpp"
#include "render_queue.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace ionengine::renderer
{
    struct RenderTaskData
    {
        core::ref_ptr<Primitive> primitive;
        math::Matrixf model;
        core::ref_ptr<Shader> shader;
        core::ref_ptr<Texture> texture;
        uint8_t mask;
    };

    struct RenderTask
    {
        core::ref_ptr<Primitive> drawable;
        math::Matrixf model;
        core::ref_ptr<Shader> shader;
        core::ref_ptr<Texture> texture;
        uint8_t mask;
    };

    template <typename Type>
    class RenderTaskIterator
    {
      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = Type*;
        using reference = Type&;

        RenderTaskIterator() : ptr(nullptr)
        {
        }

        RenderTaskIterator(pointer ptr) : ptr(ptr)
        {
        }

        reference operator*() const
        {
            return *ptr;
        }

        pointer operator->()
        {
            return ptr;
        }

        auto operator++() -> RenderTaskIterator&
        {

            ptr++;
            return *this;
        }

        auto operator++(int) -> RenderTaskIterator
        {

            RenderTaskIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend auto operator==(RenderTaskIterator const& lhs, RenderTaskIterator const& rhs) -> bool
        {

            return lhs.ptr == rhs.ptr;
        }

        friend auto operator!=(RenderTaskIterator const& lhs, RenderTaskIterator const& rhs) -> bool
        {

            return lhs.ptr != rhs.ptr;
        }

      private:
        pointer ptr;
    };

    class RenderTaskStream
    {
      public:
        using iterator = RenderTaskIterator<RenderTask>;
        using const_iterator = RenderTaskIterator<RenderTask const>;

        RenderTaskStream() = default;

        auto begin() -> iterator
        {

            return tasks.data();
        }

        auto end() -> iterator
        {

            return tasks.data() + tasks.size();
        }

        auto cbegin() const -> const_iterator
        {

            return tasks.data();
        }

        auto cend() const -> const_iterator
        {

            return tasks.data() + tasks.size();
        }

        auto write(RenderTaskData const& data) -> void
        {

            auto result = data.primitive->is_ready();
            if (result)
            {
                auto render_task = RenderTask{.drawable = data.primitive,
                                              .model = data.model,
                                              .shader = data.shader,
                                              .texture = data.texture,
                                              .mask = data.mask};
                tasks.emplace_back(std::move(render_task));
            }
        }

        auto flush() -> void
        {

            tasks.clear();
        }

        friend auto operator<<(RenderTaskStream& lhs, const RenderTaskData& rhs) -> RenderTaskStream&
        {

            lhs.write(rhs);
            return lhs;
        }

      private:
        std::vector<RenderTask> tasks;
    };

    template <std::ranges::input_range Range, typename Predicate>
    class culling_view : public std::ranges::view_interface<culling_view<Range, Predicate>>
    {
      public:
        culling_view() = default;

        culling_view(Range range, Predicate predicate) : _base(range), _iter(std::begin(_base)), _predicate(predicate)
        {
        }

        auto base() const& -> Range
        {

            return _base;
        }

        auto base() && -> Range
        {

            return std::move(_base);
        }

        auto begin()
        {

            return _iter;
        }

        auto end()
        {

            return std::end(_base);
        }

      private:
        Range _base;
        std::ranges::iterator_t<Range> _iter;
        Predicate _predicate;
    };

    template <typename Predicate>
    struct culling : std::ranges::range_adaptor_closure<culling<Predicate>>
    {
        Predicate predicate;

        culling(Predicate&& predicate) : predicate(predicate)
        {
        }

        template <std::ranges::viewable_range Range>
        auto operator()(Range&& range) const
        {
            return sort_view(range, predicate);
        }
    };
} // namespace ionengine::renderer