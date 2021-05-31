#pragma once

#include "Helpers/Logger/Logger.h"

#define noop (void)0

#ifdef _DEBUG
	// #define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#define NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
	#define NEW new
#endif

#ifndef SAFE_DELETE
	#define SAFE_DELETE(x) if(x) delete x; x = nullptr;
#endif

#ifndef SAFE_DELETE_ARRAY
	#define SAFE_DELETE_ARRAY(x) if (x) delete[] x; x = nullptr; 
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if(x) x->Release(); x=NULL;
#endif

// logging
#ifdef _DEBUG

// TODO: set LOG to work like fmt::format and use _bstr_t_ to convert everything to str
#define LOG(tag, ...) Logger::Log(tag, std::format(##__VA_ARGS__),  __FUNCTION__, __FILE__, __LINE__)
#define LOG_INFO(...) LOG("INFO", ##__VA_ARGS__)
#define LOG_WARNING(...) LOG("WARNING", ##__VA_ARGS__)

#define ERRORBOX(isFatal, ...) Logger::Fail(std::format(##__VA_ARGS__), isFatal, __FUNCTION__, __FILE__, __LINE__)

#define LOG_ERROR(...) ERRORBOX(false, ##__VA_ARGS__)
#define LOG_FATAL(...) ERRORBOX(true, ##__VA_ARGS__)

#define ASSERT(expr) if (!(expr)) ERRORBOX(false, #expr)

#else

#define LOG(tag, str) noop;
#define ERRORBOX(str, isFatal) noop;

#endif

