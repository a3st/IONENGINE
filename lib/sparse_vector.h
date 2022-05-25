// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

template<class Type>
class SparseVector {
public:

    SparseVector() = default;

    SparseVector(SparseVector const& other) {
        _sparse = other._sparse;
        _dense = other._dense;
    }

    SparseVector(SparseVector&& other) noexcept {
        _sparse = std::move(other._sparse);
        _dense = std::move(other._dense);
    }

    SparseVector& operator=(SparseVector const& other) {
        _sparse = other._sparse;
        _dense = other._dense;
        return *this;
    }

    SparseVector& operator=(SparseVector&& other) noexcept {
        _sparse = std::move(other._sparse);
        _dense = std::move(other._dense);
        return *this;
    }

    size_t push(Type const& element) {
        if(!_dense.empty()) {
            size_t const index = _dense.back();
            _dense.pop_back();
            _sparse[index] = element;
            return index;
        } else {
            size_t const index = _sparse.size();
            _sparse.emplace_back(element);
            return index;
        }
    }

    size_t push(Type&& element) {
        if(!_dense.empty()) {
            size_t const index = _dense.back();
            _dense.pop_back();
            _sparse[index] = std::move(element);
            return index;
        } else {
            size_t const index = _sparse.size();
            _sparse.emplace_back(std::move(element));
            return index;
        }
    }

    void clear() {
        _sparse.clear();
        _dense.clear();
    }

    void erase(size_t const index) {
        _sparse[index].reset();
        _dense.emplace_back(index);
    }

    Type const& get(size_t const index) const {
        return _sparse.at(index).value();
    }

    Type& get(size_t const index) {
        return _sparse[index].value();
    }

    auto has_values() { 
        return _sparse | std::views::filter([&](auto& element) { return element.has_value(); });
    }

    auto has_values() const { 
        return _sparse | std::views::filter([&](auto& element) { return element.has_value(); });
    }

    bool is_valid(size_t const index) const {
        if(index == std::numeric_limits<size_t>::max()) {
            return false;
        }
        return _sparse[index].has_value();
    }

private:

    std::vector<size_t> _dense;
    std::vector<std::optional<Type>> _sparse;
};

}
