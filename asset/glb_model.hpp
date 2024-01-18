// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/exception.hpp"

namespace ionengine::glb {

inline uint32_t const GLB_MAGIC = 0x46546C67;

struct Header {
	uint32_t magic;
	uint32_t version;
	uint32_t length;
};

struct ChunkHeader {
	uint32_t chunk_length;
	uint32_t chunk_type;
};

enum class AccessorType { Vec3, Vec2, Scalar };

struct AccessorData {
	uint32_t buffer_view;
	uint32_t count;
	AccessorType type;
};

struct BufferViewData {
	uint32_t buffer;
	size_t length;
	uint64_t offset;
};

struct AttributeData {
	std::string attrib_name;
	uint32_t accessor;
};

struct PrimitiveData {
	std::vector<AttributeData> attributes;
	uint32_t indices;
	uint32_t material;
};

struct MeshData {
	std::string name;
	std::vector<PrimitiveData> primitives;
};

class GLBModel {
public:

    GLBModel(std::span<uint8_t> const data_bytes);

    auto get_meshes() -> std::span<MeshData const> {
        
        return meshes;
    }

	auto get_buffer(uint32_t const index) -> std::span<uint8_t const> {

		return buffers[index];
	}

	auto get_accessors() -> std::span<AccessorData const> {
        
        return accessors;
    }

	auto get_buffer_views() -> std::span<BufferViewData const> {

		return buffer_views;
	}

private:

    auto get_accesor_type_by_string(std::string_view const type) -> AccessorType;

    std::vector<AccessorData> accessors;
    std::vector<BufferViewData> buffer_views;
    std::vector<MeshData> meshes;
    std::vector<std::vector<uint8_t>> buffers;
};

}
