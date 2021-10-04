// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

/**
    @brief Resource (backend::base)

    Resource base template class
*/

template<class B = backend::base>
class Resource {
public:

    ResourceType get_type() const;
};

template<class B = backend::base>
const SamplerDesc& get_sampler_desc(Resource<B>* resource);

template<class B = backend::base>
const ResourceDesc& get_resource_desc(Resource<B>* resource);

template<class B = backend::base>
byte* map(Resource<B>* resource);

template<class B = backend::base>
void unmap(Resource<B>* resource);

}