// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

template<class Type>
class SparseVector {
public:

    class Iterator {
    public:

        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = Type;
        using pointer           = Type*;
        using reference         = Type&;

        Iterator(pointer ptr) : _ptr(ptr) { }

        reference operator*() const {
            return *_ptr;
        }

        pointer operator->() {
            return _ptr;
        }

        Iterator& operator++() {
            while(!_ptr->has_value()) {
                _ptr++;
            }
            return *this; 
        }  

        Iterator operator++(Type) {
            Iterator tmp = *this;
            while(!_ptr->has_value()) {
                ++(*this);
            }
            return tmp; 
        }

        bool operator==(Iterator const& other) { 
            return _ptr == other._ptr; 
        };

        bool operator!=(Iterator const& other) { 
            return _ptr != other._ptr; 
        };

    private:
        pointer _ptr;
    };

    SparseVector() = default;

    size_t push(Type const& element) {

        if(!_sparse.empty()) {
            size_t const index = _sparse.back();
            _sparse.pop_back();
            _dense[index] = element;
            return index;
        } else {
            size_t const index = _dense.size();
            _dense.emplace_back(element);
            return index;
        }
    }

    size_t push(Type&& element) {

        if(!_sparse.empty()) {

            size_t const index = _sparse.back();
            _sparse.pop_back();
            _dense[index] = std::move(element);
            return index;
        } else {

            size_t const index = _dense.size();
            _dense.emplace_back(std::move(element));
            return index;
        }
    }

    void clear() {

        _sparse.clear();
        _dense.clear();
    }

    void erase(size_t const index) {

        _dense[index] = std::nullopt;
        _sparse.emplace_back(index);
    }

    Type const& get(size_t const index) const {

        return _dense.at(index).value();
    }

    Type& get(size_t const index) {

        return _dense[index].value();
    }

    bool is_valid(size_t const index) const {

        if(index == std::numeric_limits<size_t>::max()) {
            return false;
        }
        return _dense[index].has_value();
    }

    Iterator begin() {
        return Iterator(&_dense.at(0));
    }

    Iterator end() {
        return Iterator(&_dense.at(_dense.size()));
    }

private:

    std::vector<std::optional<Type>> _dense;
    std::vector<size_t> _sparse;
};

}
