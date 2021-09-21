// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

/**
    @brief Graphics API Adapter class

    Adapter class for Graphics API. It's needed for to get adapter information and create a device
*/
class Adapter {
public:

    virtual ~Adapter() = default;

    /**
        @brief Get the name of adapter
        @return const std::string& name of adapter
    */
    virtual const std::string& get_name() const = 0;

    /**
        @brief Get the memory of adapter
        @return usize memory in bytes
    */
    virtual usize get_memory() const = 0;

    /**
        @brief Get the device id of adapter
        @return uint32 device id of adapter
    */
    virtual uint32 get_device_id() const = 0;

    /**
        @brief Get the vendor id of adapter
        @return uint32 vendor id of adapter
    */
    virtual uint32 get_vendor_id() const = 0;

    /**
        @brief Create a device from this adapter
        @return std::unique_ptr<Device> unique pointer to device
    */
    virtual std::unique_ptr<Device> create_device() = 0;
};

}