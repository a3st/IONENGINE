// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

class ScopeProfiler {
public:

    ScopeProfiler(std::string_view const function_name);

    ~ScopeProfiler();

private:

    std::chrono::steady_clock::time_point _begin_time;

    std::string _function_name;

};

#ifdef IONENGINE_PROFILE_MODE
inline bool SCOPE_PROFILE_MODE = true;
#else
inline bool SCOPE_PROFILE_MODE = false;
#endif

#ifndef SCOPE_PROFILE
#define SCOPE_PROFILE() if(lib::SCOPE_PROFILE_MODE) lib::ScopeProfiler(__FUNCTION__);
#endif // SCOPE_PROFILE

}
