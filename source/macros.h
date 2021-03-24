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

#define LOG(tag, str) Logger::Log(tag, std::string(str),  __FUNCTION__, __FILE__, __LINE__)
#define LOG_INFO(str) LOG("INFO", str)
#define LOG_WARNING(str) LOG("WARNING", str)

#define ERRORBOX(str, isFatal) \
	do \
	{ \
		static auto msger = NEW Logger::ErrorMessenger; \
		msger->Show(std::string(str), isFatal, __FUNCTION__, __FILE__, __LINE__); \
	} \
	while(0) \

#define LOG_ERROR(str) ERRORBOX(str, false)
#define LOG_FATAL(str) ERRORBOX(str, true)

#define ASSERT(expr) if (!(expr)) ERRORBOX(#expr, false);

#else

#define LOG(tag, str) noop;
#define ERRORBOX(str, isFatal) noop;

#endif

