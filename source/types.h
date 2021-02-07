#pragma once

// Rust data types in C++ for Windows and MacOS

#if defined(_WIN32) | defined(_WIN64)

#include <WinDef.h>	

// typedef u2 u2
// typedef BYTE u8;
// typedef WORD u16;
// typedef DWORD u32;
// typedef DWORDLONG u64;

typedef INT8 i8;
typedef INT16 i16;
typedef INT32 i32;
typedef INT64 i64;

typedef UINT8 u8;
typedef UINT16 u16;
typedef UINT32 u32;
typedef UINT64 u64;

typedef FLOAT f32;
typedef double f64; // == long double in Windows

#endif // defined(_WIN32) | defined(_WIN64)
