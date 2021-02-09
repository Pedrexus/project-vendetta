#pragma once

#include <pch.h>

#include "Helpers/Classes/noncopyable.h"

#include "Helpers/Concurrency/CriticalSection.h"

#include "Helpers/Logger/Logger.h"
#include "Helpers/Logger/LoggingManager.h"

#include "Helpers/ZipFile/ZipFile.h"

extern void SetMemoryChecks();
extern std::string StringToLower(std::string s);
extern bool WildcardMatch(const char* pat, const char* str);
