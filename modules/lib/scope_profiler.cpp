// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <lib/scope_profiler.h>

using namespace ionengine;
using namespace ionengine::lib;

ScopeProfiler::ScopeProfiler(std::string_view const function_name) : 
    _function_name(function_name) {

    _begin_time = std::chrono::high_resolution_clock::now();
}

ScopeProfiler::~ScopeProfiler() {

    auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::micro> delta_time = end_time - _begin_time;

    std::cout << std::format("[Profiler] {} | delta-time execution: {} microsec", _function_name, delta_time.count()) << std::endl;
}