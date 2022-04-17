// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

class AssetManager {
public:

    AssetManager();

    template<class Type>
    std::shared_ptr<Type> load(std::filesystem::path const& file_path) {

        return std::make_shared<Type>(file_path);
    } 

private:

};

}