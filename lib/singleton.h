// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#ifndef DECLARE_SINGLETON
#define DECLARE_SINGLETON(Class) friend class Singleton<Class>;
#endif

namespace ionengine {

template<class T>
class Singleton {
public:

    static T& get_instance() {
        if(!m_instance) {
            m_instance = std::unique_ptr<T>(new T());
        }
        return *m_instance.get();
    }

protected:

    Singleton() = default;

    Singleton(const Singleton&) { 
    }

    Singleton& operator=(const Singleton&) {
    }

private:

    inline static std::unique_ptr<T> m_instance;

};

}