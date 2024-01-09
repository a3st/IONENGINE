// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "core/exception.hpp"
#include "platform/window.hpp"
#include "platform/window_loop.hpp"
#include "renderer/renderer.hpp"

using namespace ionengine;

int main(int* argc, char** agrv) {

    try {
        platform::WindowLoop loop;
        platform::Window window("Project", 800, 600, false);

        renderer::Renderer renderer(window);

        loop.run(
            window,
            [&](platform::WindowEvent const& event, platform::WindowEventFlow& flow) {
                flow = platform::WindowEventFlow::Poll;

                auto event_visitor = make_visitor(
                    [&](platform::WindowEventData<platform::WindowEventType::Closed> const& data) {
                        flow = platform::WindowEventFlow::Exit;
                    },
                    [&](platform::WindowEventData<platform::WindowEventType::Updated> const& data) {

                        renderer.render();
                    },
                    [&](platform::WindowEventData<platform::WindowEventType::Sized> const& data) {
                        
                        renderer.resize(data.width, data.height);
                    },
                    [&](platform::WindowEventData<platform::WindowEventType::KeyboardInput> const& data) {

                    },
                    [&](platform::WindowEventData<platform::WindowEventType::MouseMoved> const& data) {

                    },
                    [&](platform::WindowEventData<platform::WindowEventType::MouseInput> const& data) {

                    }
                );

                std::visit(event_visitor, event.data);
            }
        );
        
        /*asset::AssetManager asset_manager;
        renderer::Renderer renderer(window, asset_manager);
        ui::UserInterface user_interface(renderer, window);
        input::InputManager input_manager;
        scene::Scene scene(asset_manager);

        framework::Framework framework(asset_manager, input_manager, scene, user_interface);

        auto begin_time = std::chrono::high_resolution_clock::now();
        uint64_t frame_count = 0;
        uint64_t frame_count_previous = 0;
        float frame_timer = 0.0f;

        bool debug_mode = false;
        bool cursor = true;
        bool ui_debug = false;
        
        lib::logger().log(lib::LoggerCategoryType::Engine, "engine initialized");

        renderer::backend::AdapterDesc adapter_desc = renderer.adapter_desc();
        user_interface.element_text_gpu_name(adapter_desc.name);

        loop.run(
            window,
            [&](platform::WindowEvent const& event, platform::WindowEventFlow& flow) {
                flow = platform::WindowEventFlow::Poll;

                auto event_visitor = make_visitor(
                    [&](platform::WindowEventData<platform::WindowEventType::Closed> const& data) {
                        flow = platform::WindowEventFlow::Exit;
                    },
                    [&](platform::WindowEventData<platform::WindowEventType::Updated> const& data) {
                        auto end_time = std::chrono::high_resolution_clock::now();
                        std::chrono::duration<float> delta_time = end_time - begin_time;
                        begin_time = end_time;

                        lib::logger().throw_messages();

                        framework.update(delta_time.count());

                        frame_timer += delta_time.count();
                        if(frame_timer >= 1.0f) {
                            user_interface.element_text_fps(std::format("{}", frame_count - frame_count_previous));
                            user_interface.element_text_frame_time(std::format("{:.2f}ms", 1000.0f / (frame_count - frame_count_previous)));
                            frame_timer = 0.0f;
                            frame_count_previous = frame_count;

                            renderer::backend::AdapterDesc adapter_desc = renderer.adapter_desc();
                            user_interface.element_text_gpu_memory_usage(std::format("{} / {} MB", adapter_desc.local_memory_usage / (1024 * 1024), adapter_desc.local_memory_size / (1024 * 1024)));
                        }

                        user_interface.element_text_frame_count(std::format("{}", frame_count));

                        if(input_manager.key_down(input::KeyCode::Escape)) {
                            flow = platform::WindowEventFlow::Exit;
                        }

                        if(input_manager.key_down(input::KeyCode::F1)) {
                            window.cursor(cursor);
                            cursor = !cursor;
                        }

                        if(input_manager.key_down(input::KeyCode::F2)) {
                            user_interface.show_debugger(ui_debug);
                            ui_debug = !ui_debug;
                        }

                        if(input_manager.key_down(input::KeyCode::F3)) {
                            renderer.mesh_renderer()._is_ssr_enable = !renderer.mesh_renderer()._is_ssr_enable;
                            std::cout << renderer.mesh_renderer()._is_ssr_enable << std::endl;
                        }

                        if(input_manager.key_down(input::KeyCode::F4)) {
                            user_interface.show_debug(debug_mode);
                            debug_mode = !debug_mode;
                        }

                        user_interface.update();
                        input_manager.update();
                        scene.update(delta_time.count());
                        asset_manager.update(delta_time.count());
                        renderer.update(delta_time.count());
                        
                        renderer.render(scene, user_interface);

                        ++frame_count;
                    },
                    [&](platform::WindowEventData<platform::WindowEventType::Sized> const& data) {
                        renderer.resize(data.width, data.height);

                        scene::CameraNode* camera = scene.find_by_name<scene::CameraNode>("main_camera");
                        camera->aspect_ratio(data.width / static_cast<float>(data.height));
                    },
                    [&](platform::WindowEventData<platform::WindowEventType::KeyboardInput> const& data) {
                        input_manager.on_keyboard_event(data);
                    },
                    [&](platform::WindowEventData<platform::WindowEventType::MouseMoved> const& data) {
                        input_manager.on_mouse_moved_event(data);
                        user_interface.context().ProcessMouseMove(data.x, data.y, 0);
                    },
                    [&](platform::WindowEventData<platform::WindowEventType::MouseInput> const& data) {
                        if(data.input_state == platform::InputState::Pressed) {
                            switch(data.button) {
                                case platform::MouseButton::Left: {
                                    user_interface.context().ProcessMouseButtonDown(0, 0);
                                } break;
                                case platform::MouseButton::Right: {
                                    user_interface.context().ProcessMouseButtonDown(1, 0);
                                } break;
                            }
                        } else {
                            switch(data.button) {
                                case platform::MouseButton::Left: {
                                    user_interface.context().ProcessMouseButtonUp(0, 0);
                                } break;
                                case platform::MouseButton::Right: {
                                    user_interface.context().ProcessMouseButtonUp(1, 0);
                                } break;
                            }
                        }
                    }
                );

                std::visit(event_visitor, event.data);
            }
        );*/

    } catch(core::Exception e) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
