#pragma once

#include <pch.h>
#include <types.h>

// constants to be used throghout the project

constexpr u64 KILOBYTE = 1024;
constexpr u64 MEGABYTE = 1024 * 1024;

// project requirements and specifications

constexpr auto GAME_TITLE = L"Project Vendetta";

// hardware requirements

constexpr auto PHYSICAL_RAM = 512 * MEGABYTE;
constexpr auto VIRTUAL_RAM = 1024 * MEGABYTE;
constexpr auto DISK_SPACE = 10 * MEGABYTE;
constexpr auto CPU_SPEED = 1300; // 1.3GHz

// window specifications

constexpr auto WINDOW_WIDTH = 800;
constexpr auto WINDOW_HEIGHT = 600;

constexpr auto WINDOW_CLASS_NAME = L"Project_Vendetta_GameWindowClass";
constexpr auto WINDOW_TITLE_NAME = L"Project Vendetta Game";
constexpr auto WINDOW_EXTENDED_STYLE = WS_EX_LEFT;
constexpr auto WINDOW_STYLE = WS_OVERLAPPEDWINDOW;
constexpr auto WINDOW_HAS_MENU = FALSE;

// dependencies

constexpr auto RESOURCES_ZIPFILE = L"Assets.zip";
constexpr auto RESOURCES_SIZE = 50 * MEGABYTE;

constexpr auto SAVE_GAME_DIR = L"ProjectVendetta";

// logger display flags

constexpr auto LOGGING_CONFIG_FILENAME = "Logging.xml";
constexpr auto LOGGING_OUTPUT_FILENAME = "vendetta.log";
