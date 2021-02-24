#include "Singleton.h"

#include <macros.h>

Singleton* Singleton::GetInstance()
{
	std::lock_guard<std::mutex> lock(mutex);

	if (instance == nullptr)
		instance = NEW Singleton();

	return instance;
}
