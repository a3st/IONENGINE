// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    template <typename Type>
    class ring_buffer
    {
      public:
        ring_buffer(uint32_t const size) : buffer(size), tail(0), head(0), _size(0)
        {
        }

        ring_buffer(ring_buffer const& other)
            : buffer(other.buffer), head(other.head), tail(other.tail), _size(other._size)
        {
        }

        ring_buffer(ring_buffer&& other) : buffer(std::move(other.buffer)), head(other.head), tail(other.tail), _size(other._size)
        {
        }

        auto operator=(ring_buffer const& other) -> ring_buffer&
        {
            buffer = other.buffer;
            head = other.head;
            tail = other.tail;
            _size = other._size;
            return *this;
        }

        auto operator=(ring_buffer&& other) -> ring_buffer&
        {
            buffer = std::move(other.buffer);
            head = other.head;
            tail = other.tail;
            _size = other._size;
            return *this;
        }

        auto push(Type const& element) -> void
        {
            buffer[tail] = element;
            increment_tail();
        }

        auto pop() -> void
        {
            assert(_size > 0 && "ring_buffer is empty");
            increment_head();
        }

        auto front() -> Type
        {
            assert(_size > 0 && "ring_buffer is empty");
            return buffer[head];
        }

        auto back() -> Type
        {
            assert(_size > 0 && "ring_buffer is empty");
            return buffer[tail];
        }

        auto capacity() const -> size_t
        {
            return buffer.size();
        }

        auto size() const -> size_t
        {
            return _size;
        }

        auto empty() const -> bool
        {
            return _size == 0;
        }

      private:
        std::vector<Type> buffer;
        size_t head;
        size_t tail;
        size_t _size;

        auto increment_tail() -> void
        {
            tail = (tail + 1) % buffer.size();
            if (_size != buffer.size())
            {
                _size++;
            }
        }

        auto increment_head() -> void
        {
            head = (head + 1) % buffer.size();
            _size--;
        }
    };
} // namespace ionengine::core