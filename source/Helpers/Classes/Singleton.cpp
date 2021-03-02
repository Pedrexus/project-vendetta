#include "Singleton.h"

Singleton* Singleton::Get()
{
	std::lock_guard<std::mutex> lock(mutex);

	if (instance == nullptr)
	{
		LOG("INFO", "Singleton instantiated");
		instance = NEW Singleton();
	}

	return instance;
}
