
#pragma once

namespace ionengine::lib {

template<class Type>
class SparseVector {
public:

    SparseVector();

    

private:

    std::vector<std::optional<Type>> _dense;
    std::vector<size_t> _sparse;
};