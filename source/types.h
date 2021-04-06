#pragma once

#include <cstdint>

// Windows types
// typedef u2 u2
// typedef BYTE u8;
// typedef WORD u16;
// typedef DWORD u32;
// typedef DWORDLONG u64;

// Rust typing naming in C++ for Windows and MacOS

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

// there is also f80 = long long double 
// in Unix systems, but I won't use it

typedef f64 milliseconds;
constexpr auto INFINITE_TIME = UINT32_MAX;

typedef u32 size; // buffer size, file size, etc.
