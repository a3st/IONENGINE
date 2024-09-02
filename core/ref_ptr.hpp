// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    class ref_counted_object
    {
      public:
        ref_counted_object() : ref_count(0)
        {
        }

        virtual ~ref_counted_object() = default;

        ref_counted_object(ref_counted_object const& other) : ref_count(other.ref_count.load())
        {
        }

        ref_counted_object(ref_counted_object&& other) : ref_count(other.ref_count.load())
        {
        }

        auto operator=(ref_counted_object const& other) -> ref_counted_object&
        {
            ref_count = other.ref_count.load();
            return *this;
        }

        auto operator=(ref_counted_object&& other) -> ref_counted_object&
        {
            ref_count = other.ref_count.load();
            return *this;
        }

        auto add_ref() -> uint32_t
        {
            return ++ref_count;
        }

        auto release() -> uint32_t
        {
            return --ref_count;
        }

      private:
        std::atomic<uint32_t> ref_count;
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
        ref_ptr() : ptr(nullptr)
        {
        }

        ref_ptr(std::nullptr_t) : ptr(nullptr)
        {
        }

        ~ref_ptr()
        {
            release_ref();
        }

        ref_ptr(Type* ptr) : ptr(ptr)
        {
            add_ref();
        }

        template <typename Derived, typename DerivedDeleter = BaseDeleter<Derived>>
        ref_ptr(ref_ptr<Derived, DerivedDeleter> other) : ptr(static_cast<Type*>(other.ptr))
        {
            add_ref();
        }

        ref_ptr(ref_ptr const& other) : ptr(other.ptr)
        {
            add_ref();
        }

        auto operator=(ref_ptr const& other) -> ref_ptr&
        {
            copy_ref(other.ptr);
            return *this;
        }

        template <typename Derived, typename DerivedDeleter = BaseDeleter<Derived>>
        auto operator=(ref_ptr<Derived, DerivedDeleter> other) -> ref_ptr&
        {
            copy_ref(static_cast<Type*>(other.ptr));
            return *this;
        }

        auto operator->() const -> Type*
        {
            assert(ptr != nullptr && "ref_ptr is null");
            return ptr;
        }

        auto operator*() const -> Type&
        {
            assert(ptr != nullptr && "ref_ptr is null");
            return *ptr;
        }

        auto get() const -> Type*
        {
            assert(ptr != nullptr && "ref_ptr is null");
            return ptr;
        }

        auto release() -> Type*
        {
            release_ref();
            return ptr;
        }

        operator bool() const
        {
            return ptr != nullptr;
        }

      private:
        auto copy_ref(Type* other) -> void
        {
            if (ptr != other)
            {
                release_ref();
                ptr = other;
                add_ref();
            }
        }

        auto add_ref() -> void
        {
            if (ptr)
            {
                ptr->add_ref();
            }
        }

        auto release_ref() -> void
        {
            if (ptr)
            {
                uint32_t const count = ptr->release();
                if (count == 0)
                {
                    Deleter()(ptr);
                }
            }
        }

        Type* ptr;
    };

    template <typename Type, typename Deleter = base_deleter<Type>, typename... Args>
    inline auto make_ref(Args&&... args) -> ref_ptr<Type, Deleter>
    {
        Type* ptr = new Type(std::forward<Args>(args)...);
        return ref_ptr<Type, Deleter>(ptr);
    }
} // namespace ionengine::core