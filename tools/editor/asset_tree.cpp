// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "asset_tree.hpp"
#include "engine/asset.hpp"
#include "precompiled.h"
#include "shader_graph/shader_graph.hpp"

namespace ionengine::tools::editor
{
    AssetTree::AssetTree(std::filesystem::path const& rootPath) : rootPath(rootPath)
    {
        rootStruct = std::make_unique<AssetStructInfo>();
        rootStruct->assetName = rootPath.filename().generic_string();
        rootStruct->assetType = AssetType::Folder;
        rootStruct->assetPath = rootPath.generic_string();
    }

    auto AssetTree::fetch() -> AssetStructInfo const&
    {
        //rootStruct->childrens.clear();

        auto internalFetch = [&](this auto const& internalFetch, AssetStructInfo* curStruct,
                                 std::filesystem::path const& rootPath) -> void {
            for (auto const& dirEntry : std::filesystem::directory_iterator(rootPath))
            {
                if (std::filesystem::is_directory(dirEntry.path()))
                {
                    auto folderStruct = std::make_unique<AssetStructInfo>();
                    folderStruct->assetName = dirEntry.path().filename().generic_string();
                    folderStruct->assetPath = dirEntry.path().generic_string();
                    folderStruct->assetType = AssetType::Folder;

                    //curStruct->childrens.emplace_back(std::move(folderStruct));
                    //internalFetch(curStruct->childrens.back().get(), dirEntry.path());
                }
                else
                {
                    auto assetStruct = std::make_unique<AssetStructInfo>();
                    assetStruct->assetName = dirEntry.path().stem().generic_string();
                    assetStruct->assetPath = dirEntry.path().generic_string();

                    if (dirEntry.path().extension().compare(".asset") == 0)
                    {
                        std::basic_ifstream<uint8_t> stream(
                            std::filesystem::path(curStruct->assetPath).make_preferred(), std::ios::binary);
                        auto result = getAssetHeader(stream);
                        if (result.has_value())
                        {
                            asset::Header header = std::move(result.value());
                            if (std::memcmp(header.fileType.data(), ShaderGraphFileType.data(),
                                            ShaderGraphFileType.size()) == 0)
                            {
                                assetStruct->assetType = AssetType::ShaderGraph;
                            }
                            else
                            {
                                assetStruct->assetType = AssetType::Unknown;
                            }
                        }
                        else
                        {
                            assetStruct->assetType = AssetType::Unknown;
                        }
                    }
                    else
                    {
                        assetStruct->assetType = AssetType::Unknown;
                    }
                    //curStruct->childrens.emplace_back(std::move(assetStruct));
                }
            }
        };

        internalFetch(rootStruct.get(), rootPath);
        return *rootStruct;
    }
} // namespace ionengine::tools::editor