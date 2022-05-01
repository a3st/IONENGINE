// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

template<class Type>
class SparseVector {
public:

    SparseVector() = default;

    size_t push(Type const& element) {

        if(!_sparse.empty()) {

            size_t const index = _sparse.back();
            _sparse.pop_back();
            _dense[index] = element;
            return index;
        } else {

            size_t const index = _dense.size();
            _dense[index] = element;
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
            _dense[index] = std::move(element);
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

private:

    std::vector<std::optional<Type>> _dense;
    std::vector<size_t> _sparse;
};

}
