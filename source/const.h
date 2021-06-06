#pragma once

#include <pch.h>
#include <types.h>

// constants to be used throghout the project

constexpr u64 KILOBYTE = 1024;
constexpr u64 MEGABYTE = 1024 * 1024;

// project requirements and specifications

constexpr auto SETTINGS_FILENAME = "settings.xml";

// software execution settings

constexpr auto LOOP_EXECUTION_POLICY = std::execution::par_unseq; // parallel unsequenced policy

// window specifications

constexpr auto WINDOW_STYLE = WS_OVERLAPPEDWINDOW;
constexpr auto WINDOW_HAS_MENU = FALSE;
