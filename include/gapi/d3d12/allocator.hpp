#pragma once

#include "core/ref_ptr.hpp"
#include <d3d12.h>

namespace ie {

namespace gapi {

enum class DescriptorAllocatorFlags : uint32_t {
    None = 0,
    Reset = 1
};

class DescriptorAllocBlock : public core::ref_counted_object {
public:

private:
};

class DescriptorAllocator : public core::ref_counted_object {
public:

    DescriptorAllocator();

    

private:
};

}

}