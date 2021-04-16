#pragma once

#include <pch.h>
#include <types.h>

// constants to be used throghout the project

constexpr u64 KILOBYTE = 1024;
constexpr u64 MEGABYTE = 1024 * 1024;

// project requirements and specifications

constexpr auto GAME_TITLE = L"Project Vendetta";
constexpr auto GAME_LANGUAGE = "English";

// hardware requirements

constexpr auto PHYSICAL_RAM = 512 * MEGABYTE;
constexpr auto VIRTUAL_RAM = 1024 * MEGABYTE;
constexpr auto DISK_SPACE = 10 * MEGABYTE;
constexpr auto CPU_SPEED = 1300; // 1.3GHz

// software execution settings

constexpr auto LOOP_EXECUTION_POLICY = std::execution::par_unseq; // parallel unsequenced policy

// window specifications

constexpr auto WINDOW_WIDTH = 960;
constexpr auto WINDOW_HEIGHT = 540;
constexpr auto SCREEN_REFRESH_RATE = 1000 / 60;  // 1s / 60 = 60 fps

constexpr auto WINDOW_CLASS_NAME = L"Project_Vendetta_GameWindowClass";
constexpr auto WINDOW_TITLE_NAME = L"Project Vendetta Game";
constexpr auto WINDOW_STYLE = WS_OVERLAPPEDWINDOW;
constexpr auto WINDOW_HAS_MENU = FALSE;

// sound system specifications

constexpr auto NUMBER_PRIMARY_CHANNELS = 32;
constexpr auto PRIMARY_SAMPLE_RATE = 44100;
constexpr auto PRIMARY_AUDIO_BITRATE = 16;
constexpr auto MINIMUM_VOLUME = 0;
constexpr auto MAXIMUM_VOLUME = 100;

// graphics system specifications

#include <d3dcommon.h>

constexpr auto DXD12_MINIMUM_FEATURE_LEVEL = D3D_FEATURE_LEVEL_11_0;
constexpr auto NUMBER_FRAME_RESOURCES = 3; // TODO: set this to 5%-10% the current fps

// dependencies

constexpr auto RESOURCES_ZIPFILE = L"Assets.zip";
constexpr auto RESOURCES_SIZE = 50 * MEGABYTE;

constexpr auto SAVE_GAME_DIR = L"ProjectVendetta";

// logger display flags

constexpr auto LOGGING_CONFIG_FILENAME = "Logging.xml";
constexpr auto LOGGING_OUTPUT_FILENAME = "vendetta.log";
constexpr auto LOGGING_MODE = "clear";