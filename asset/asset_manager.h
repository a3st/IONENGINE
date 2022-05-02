// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset_ptr.h>
#include <lib/event_dispatcher.h>

namespace ionengine::asset {

class Mesh;
class Material;
class Technique;
class Texture;

enum class AssetEventType {
    Loaded,
    Unloaded,
    Added,
    Removed
};

template<class AssetType, AssetEventType EventType>
struct AssetEventData { };

template<class AssetType>
struct AssetEventData<AssetType, AssetEventType::Loaded> {
    AssetPtr<AssetType> asset;
};

template<class AssetType>
struct AssetEventData<AssetType, AssetEventType::Unloaded> {
    AssetPtr<AssetType> asset;
};

template<class AssetType>
struct AssetEventData<AssetType, AssetEventType::Added> {
    AssetPtr<AssetType> asset;
};

template<class AssetType>
struct AssetEventData<AssetType, AssetEventType::Removed> {
    std::filesystem::path path;
};

template<class Type = Asset>
struct AssetEvent {
    std::variant<
        AssetEventData<Type, AssetEventType::Loaded>,
        AssetEventData<Type, AssetEventType::Unloaded>,
        AssetEventData<Type, AssetEventType::Added>,
        AssetEventData<Type, AssetEventType::Removed>
    > data;

    static AssetEvent<Type> removed(std::filesystem::path const& path) {

        return AssetEvent<Type> {
            .data = AssetEventData<Type, AssetEventType::Removed> { 
                .path = path 
            }
        };
    }

    static AssetEvent<Type> added(AssetPtr<Type> const& asset) {

        return AssetEvent<Type> {
            .data = AssetEventData<Type, AssetEventType::Added> {
                .asset = asset
            }
        };
    }
};

template<class Type>
struct TimedEntry {
    Type value;
    float time_to_live;
};

template<class Type>
class AssetPool {
public:

    AssetPool() = default;

    AssetPtr<Type> find(std::filesystem::path const& asset_path) {

        for(auto& entry : _data) {
            if(entry.value.path() == asset_path) {
                return entry.value;
            }
        }

        return AssetPtr<Type>();
    }

    void push(AssetPtr<Type> const& element) {

        _event_dispatcher.broadcast(AssetEvent<Type>::added(element));
        std::cout << std::format("Asset {} added", element.path().string()) << std::endl;
        _data.emplace_back(element, 60.0f);
    }

    void update(float const delta_time) {

        std::erase_if(
            _data,
            [&](auto& element) {

                if(element.value.use_count() <= 1) {
                    element.time_to_live -= delta_time;
                    if(element.time_to_live <= 0.0f) {
                        std::filesystem::path path = element.value.path();
                        _event_dispatcher.broadcast(AssetEvent<Type>::removed(path));
                        std::cout << std::format("Asset {} removed", path.string()) << std::endl;
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    element.time_to_live = 60.0f;
                    return false;
                }
            }
        );
    }

    void erase_unused() {

        std::erase_if(_data, [&](auto const& element) { return element.value.use_count() > 1; });
    }

    AssetPtr<Type> get(std::filesystem::path const& asset_path) {

        AssetPtr<Type> ptr = find(asset_path);
        if(ptr) {
            return ptr;
        } else {

            AssetPtr<Type> asset(asset_path);
            push(asset);

            return asset;
        }
    }

    lib::EventDispatcher<AssetEvent<Type>>& event_dispatcher() { return _event_dispatcher; }

private:

    std::vector<TimedEntry<AssetPtr<Type>>> _data;
    lib::EventDispatcher<AssetEvent<Type>> _event_dispatcher;
};

class AssetManager {
public:

    AssetManager();

    AssetPtr<Mesh> get_mesh(std::filesystem::path const& file_path);

    AssetPtr<Technique> get_technique(std::filesystem::path const& file_path);

    void update(float const delta_time);

    AssetPool<Mesh>& mesh_pool() { return _meshes; }

private:

    AssetPool<Mesh> _meshes;
    AssetPool<Technique> _techniques;
};

}