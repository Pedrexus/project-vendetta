#pragma once

#include <pch.h>
#include <macros.h>

// thread-safe singleton
class Singleton
{
private:
    static Singleton* instance;
    static std::mutex mutex;

protected:
    Singleton() = default;
    ~Singleton() = default;

public:
    Singleton(Singleton& other) = delete; // Singletons should not be cloneable.
    void operator=(const Singleton&) = delete; // Singletons should not be assignable.

    /**
     * This is the static method that controls the access to the singleton
     * instance. On the first run, it creates a singleton object and places it
     * into the static field. On subsequent runs, it returns the client existing
     * object stored in the static field.
     */

    static Singleton* Get();
    inline Singleton* Destroy() { SAFE_DELETE(instance); };
};