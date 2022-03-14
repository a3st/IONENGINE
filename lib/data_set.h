// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

template<class Type, size_t Size>
class DataSet {
public:

    DataSet() {

       fill_data();
    }

    uint32_t push(Type const& element) {
        
        assert(!_ids.empty() && "The data set is full");

        uint32_t const index = _ids.front();
        _ids.pop_front();
        _data[index] = element;
        return index;
    }

    uint32_t push(Type&& element) {

        assert(!_ids.empty() && "The data set is full");

        uint32_t const index = _ids.front();
        _ids.pop_front();
        _data[index] = std::move(element);
        return index;
    }

    void erase(uint32_t const index) {

        _ids.emplace_back(index);
        _data[index] = Type {};
    }

    void clear() {

        _data.clear();
        _ids.clear();

        fill_data();
    }

    Type const& operator[](uint32_t const index) const {

        return _data[index];
    }

    Type& operator[](uint32_t const index) {

        return _data[index];
    }

private:

    std::vector<Type> _data;
    std::list<uint32_t> _ids;

    void fill_data() {

        _data.resize(Size);

        for(uint32_t i = 0; i < Size; ++i) {

            _ids.emplace_back(i);
        }
    }
};

}
