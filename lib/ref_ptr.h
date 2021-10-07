// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

template<class T>
class RefPtr {
public:

    RefPtr(T* ptr);

    ~RefPtr();

    RefPtr(const RefPtr& rhs);
    
    RefPtr(RefPtr&& rhs);

    RefPtr& operator=(const RefPtr& rhs);
    
    RefPtr& operator=(RefPtr&& ptr);
};

}