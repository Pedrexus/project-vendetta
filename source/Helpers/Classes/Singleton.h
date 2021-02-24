#pragma once

#include <pch.h>

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
    // Singletons should not be cloneable.
    Singleton(Singleton& other) = delete;

    // Singletons should not be assignable.
    void operator=(const Singleton&) = delete;

    /**
     * This is the static method that controls the access to the singleton
     * instance. On the first run, it creates a singleton object and places it
     * into the static field. On subsequent runs, it returns the client existing
     * object stored in the static field.
     */

    static Singleton* GetInstance();
};