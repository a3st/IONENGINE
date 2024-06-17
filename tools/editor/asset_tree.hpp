// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::tools::editor
{
    enum class AssetType
    {
        Unknown,
        Asset,
        Folder
    };

    struct AssetStructInfo
    {
        std::string name;
        std::filesystem::path path;
        AssetType type;
        std::vector<std::unique_ptr<class AssetStructInfo>> childrens;
    };

    class AssetTree
    {
      public:
        AssetTree(std::filesystem::path const& rootPath);

        auto fetch() -> AssetStructInfo const&;

        auto createFile(std::filesystem::path const& filePath, std::basic_ofstream<uint8_t>& stream) -> bool;

        auto deleteFile(std::filesystem::path const& filePath) -> bool;

      private:
        std::unique_ptr<AssetStructInfo> rootStruct;
        std::filesystem::path rootPath;
    };
} // namespace ionengine::tools::editor