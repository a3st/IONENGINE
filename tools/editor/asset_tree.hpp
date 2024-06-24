// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/serializable.hpp"

namespace ionengine::tools::editor
{
    enum class AssetType
    {
        Unknown,
        ShaderGraph,
        Folder
    };

    struct AssetStructInfo
    {
        std::string assetName;
        std::string assetPath;
        AssetType assetType;
        //std::vector<std::unique_ptr<class AssetStructInfo>> childrens;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(assetName, "name");
            archive.property(assetType, "type");
            archive.property(assetPath, "path");
            //archive.property(childrens, "childrens");
        }
    };

    class AssetTree
    {
      public:
        AssetTree(std::filesystem::path const& rootPath);

        auto fetch() -> AssetStructInfo const&;

      private:
        std::unique_ptr<AssetStructInfo> rootStruct;
        std::filesystem::path rootPath;
    };
} // namespace ionengine::tools::editor

namespace ionengine::core
{
    template <>
    struct SerializableEnum<tools::editor::AssetType>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(tools::editor::AssetType::Unknown, "file/unknown");
            archive.field(tools::editor::AssetType::ShaderGraph, "asset/shadergraph");
            archive.field(tools::editor::AssetType::Folder, "folder");
        }
    };
} // namespace ionengine::core