// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset_event.h>
#include <asset/asset_ptr.h>
#include <asset/asset_loader.h>
#include <asset/technique_loader.h>
#include <asset/mesh_loader.h>
#include <lib/event_dispatcher.h>

namespace ionengine::asset {

class Mesh;
class Material;
class Technique;
class Texture;

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
        return nullptr;
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

        AssetPtr<Type> asset = find(asset_path);
        if(asset) {
            return asset;
        } else {

            asset = AssetPtr<Type>(asset_path);
            push(asset);

            _loader.load_asset(asset, _event_dispatcher);

            return asset;
        }
    }

    lib::EventDispatcher<AssetEvent<Type>>& event_dispatcher() { return _event_dispatcher; }

private:

    std::vector<TimedEntry<AssetPtr<Type>>> _data;
    lib::EventDispatcher<AssetEvent<Type>> _event_dispatcher;
    AssetLoader<Type> _loader;
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
