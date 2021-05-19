#pragma once


#include <pch.h>

template <class T>
class Asynchronous
{
	std::unique_ptr<T> obj;

	inline Asynchronous()
	{
		obj = std::make_unique<T>();
	}


};