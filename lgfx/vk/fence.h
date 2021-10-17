// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class VKFence : public Fence {
public:

    VKFence(vk::Device* vk_device, const uint64 initial_value) : m_vk_device(vk_device) {

        vk::SemaphoreTypeCreateInfo semaphore_type_create_info{};
        semaphore_type_create_info.initialValue = initial_value;
        semaphore_type_create_info.semaphoreType = vk::SemaphoreType::eTimeline;

        vk::SemaphoreCreateInfo semaphore_create_info;
        semaphore_create_info.pNext = &semaphore_type_create_info;
        m_semaphore = m_vk_device->createSemaphoreUnique(semaphore_create_info);
    }

    uint64 get_completed_value() const override { return m_vk_device->getSemaphoreCounterValueKHR(m_semaphore.get()); }

    void wait(const uint64 value) override {

        vk::SemaphoreWaitInfo semaphore_wait_info{};
        semaphore_wait_info.semaphoreCount = 1;
        semaphore_wait_info.pSemaphores = &m_semaphore.get();
        semaphore_wait_info.pValues = &value;
        THROW_IF_FAILED(m_vk_device->waitSemaphoresKHR(semaphore_wait_info, std::numeric_limits<uint64>::max()));
    }

    void signal(const uint64 value) override {  

        vk::SemaphoreSignalInfo semaphore_signal_info = {};
        semaphore_signal_info.semaphore = m_semaphore.get();
        semaphore_signal_info.value = value;
        m_vk_device->signalSemaphoreKHR(semaphore_signal_info);
    }

private:

    vk::Device* m_vk_device;
    vk::UniqueSemaphore m_semaphore;
};

}