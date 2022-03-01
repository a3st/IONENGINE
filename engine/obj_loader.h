// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/math/vector.h>

namespace ionengine {

struct ObjIndex {
    uint32_t vertex;
    uint32_t uv;
    uint32_t normal;
};

struct ObjMesh {
    std::vector<Vector3f> vertices;
    std::vector<Vector3f> normals;
    std::vector<Vector2f> uvs;
    std::vector<ObjIndex> indices;
};

class ObjLoader {
public:

    /*class Iterator {
    public:

        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = ObjMesh;
        using pointer           = ObjMesh*;
        using reference         = ObjMesh&;

        Iterator(pointer ptr) : _ptr(ptr) {}

        reference operator*() const { return *_ptr; }
        pointer operator->() { return _ptr; }

        Iterator& operator++() { _ptr++; return *this; }  
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

        bool operator==(Iterator const& other) const { return _ptr == other._ptr; }
        bool operator!=(Iterator const& other) const { return _ptr != other._ptr; }

    private:

        pointer _ptr;
    };*/

    ObjLoader() = default;

    bool parse(std::span<char8_t> const data);

    std::vector<ObjMesh>::iterator begin() { return _meshes.begin(); }
    std::vector<ObjMesh>::iterator end() { return _meshes.end(); }

private:

    std::vector<ObjMesh> _meshes;
};

}