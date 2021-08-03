// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Adapter {
public:

    virtual ~Adapter() = default;
    virtual const std::string& get_name() const = 0;
    virtual usize get_memory() const = 0;
    virtual uint32 get_device_id() const = 0;
    virtual uint32 get_vendor_id() const = 0;
    virtual std::unique_ptr<Device> create_device() = 0;
};

}