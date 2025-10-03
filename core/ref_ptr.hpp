// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    class ref_counted_object
    {
      public:
        ref_counted_object() : _ref_count(0)
        {
        }

        virtual ~ref_counted_object() = default;

        ref_counted_object(ref_counted_object const& other) : _ref_count(other._ref_count.load())
        {
        }

        ref_counted_object(ref_counted_object&& other) : _ref_count(other._ref_count.load())
        {
        }

        auto operator=(ref_counted_object const& other) -> ref_counted_object&
        {
            _ref_count = other._ref_count.load();
            return *this;
        }

        auto operator=(ref_counted_object&& other) -> ref_counted_object&
        {
            _ref_count = other._ref_count.load();
            return *this;
        }

        auto add_ref() -> uint32_t
        {
            return ++_ref_count;
        }

        auto release() -> uint32_t
        {
            return --_ref_count;
        }

        auto use_count() const -> uint32_t
        {
            return _ref_count;
        }

      private:
        std::atomic<uint32_t> _ref_count;
    };

    template <typename Type>
    struct base_deleter
    {
        auto operator()(Type* ptr) -> void
        {
            delete ptr;
        }
    };

    template <typename Type, typename Deleter = base_deleter<Type>>
    class ref_ptr
    {
        template <typename Derived, typename DerivedDeleter>
        friend class ref_ptr;

      public:
        ref_ptr() : _ptr(nullptr)
        {
        }

        ref_ptr(std::nullptr_t) : _ptr(nullptr)
        {
        }

        ~ref_ptr()
        {
            release_ref();
        }

        ref_ptr(Type* ptr) : _ptr(ptr)
        {
            add_ref();
        }

        template <typename Derived, typename DerivedDeleter = BaseDeleter<Derived>>
        ref_ptr(ref_ptr<Derived, DerivedDeleter> other) : _ptr(static_cast<Type*>(other._ptr))
        {
            add_ref();
        }

        ref_ptr(ref_ptr const& other) : _ptr(other._ptr)
        {
            add_ref();
        }

        auto operator=(ref_ptr const& other) -> ref_ptr&
        {
            copy_ref(other._ptr);
            return *this;
        }

        template <typename Derived, typename DerivedDeleter = BaseDeleter<Derived>>
        auto operator=(ref_ptr<Derived, DerivedDeleter> other) -> ref_ptr&
        {
            copy_ref(static_cast<Type*>(other._ptr));
            return *this;
        }

        auto operator->() const -> Type*
        {
            assert(_ptr != nullptr && "ref_ptr is null");
            return _ptr;
        }

        auto operator*() const -> Type&
        {
            assert(_ptr != nullptr && "ref_ptr is null");
            return *_ptr;
        }

        auto get() const -> Type*
        {
            assert(_ptr != nullptr && "ref_ptr is null");
            return _ptr;
        }

        auto release() -> Type*
        {
            Type* releasePtr = _ptr;
            _ptr = nullptr;
            return releasePtr;
        }

        operator bool() const
        {
            return _ptr != nullptr;
        }

        auto operator==(ref_ptr const& other) const -> bool
        {
            return _ptr == other._ptr;
        }

        auto operator!=(ref_ptr const& other) const -> bool
        {
            return _ptr != other._ptr;
        }

      private:
        auto copy_ref(Type* other) -> void
        {
            if (_ptr != other)
            {
                release_ref();
                _ptr = other;
                add_ref();
            }
        }

        auto add_ref() -> void
        {
            if (_ptr)
            {
                _ptr->add_ref();
            }
        }

        auto release_ref() -> void
        {
            if (_ptr)
            {
                uint32_t const count = _ptr->release();
                if (count == 0)
                {
                    Deleter()(_ptr);
                }
            }
        }

        Type* _ptr;
    };

    template <typename Type>
    class weak_ptr
    {
      public:
        weak_ptr() : _ptr(nullptr)
        {
        }

        weak_ptr(std::nullptr_t) : _ptr(nullptr)
        {
        }

        weak_ptr(ref_ptr<Type> const& ptr) : _ptr(ptr.get())
        {
        }

        weak_ptr(weak_ptr const& other) : _ptr(other._ptr)
        {
        }

        auto operator=(weak_ptr const& other) -> weak_ptr&
        {
            _ptr = other._ptr;
            return *this;
        }

        auto operator->() -> ref_ptr<Type>
        {
            assert(_ptr != nullptr && "weak_ptr is null");
            return _ptr;
        }

        auto get() const -> ref_ptr<Type>
        {
            assert(_ptr != nullptr && "weak_ptr is null");
            return _ptr;
        }

        operator bool() const
        {
            return _ptr != nullptr;
        }

        auto operator==(weak_ptr const& other) const -> bool
        {
            return _ptr == other._ptr;
        }

        auto operator!=(weak_ptr const& other) const -> bool
        {
            return _ptr == other._ptr;
        }

      private:
        Type* _ptr;
    };

    template <typename Type, typename Deleter = base_deleter<Type>, typename... Args>
    inline auto make_ref(Args&&... args) -> ref_ptr<Type, Deleter>
    {
        Type* ptr = new Type(std::forward<Args>(args)...);
        return ref_ptr<Type, Deleter>(ptr);
    }
} // namespace ionengine::core

template <typename Type>
struct std::hash<ionengine::core::ref_ptr<Type>>
{
    auto operator()(ionengine::core::ref_ptr<Type> const& other) const -> size_t
    {
        return std::hash<uintptr_t>()((uintptr_t)other.get());
    }
};