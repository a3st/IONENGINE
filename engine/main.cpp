
#include "core/exception.hpp"
#include "platform/window.hpp"
#include "precompiled.h"
#include "rhi/rhi.hpp"

using namespace ionengine;

auto main(int32_t argc, char** argv) -> int32_t
{
    try
    {
        auto window = platform::Window::create("Example", 800, 600);

        auto device = rhi::Device::create(window.get());

        auto graphics_context = device->create_graphics_context();
        auto copy_context = device->create_copy_context();

        bool is_running = true;
        platform::WindowEvent event;
        while (is_running)
        {
            while (window->try_get_event(event))
            {
                utils::variant_match(event).case_<platform::WindowEventData<platform::WindowEventType::Closed>>(
                    [&](auto& data) { is_running = false; });
            }

            auto back_buffer = device->request_back_buffer();

            graphics_context->reset();

            std::vector<rhi::RenderPassColorInfo> colors = {
                rhi::RenderPassColorInfo{.texture = back_buffer,
                                         .load_op = rhi::RenderPassLoadOp::Clear,
                                         .store_op = rhi::RenderPassStoreOp::Store,
                                         .clear_color = math::Color(0.2f, 0.3f, 0.5f, 1.0f)}};

            graphics_context->barrier(back_buffer, rhi::ResourceState::Common, rhi::ResourceState::RenderTarget);
            graphics_context->begin_render_pass(colors, std::nullopt);
            graphics_context->end_render_pass();
            graphics_context->barrier(back_buffer, rhi::ResourceState::RenderTarget, rhi::ResourceState::Common);

            rhi::Future<rhi::Query> result = graphics_context->execute();

            device->present_back_buffer();

            result.wait();
        }
    }
    catch (core::Exception e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}