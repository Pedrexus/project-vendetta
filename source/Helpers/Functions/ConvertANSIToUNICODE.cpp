#pragma once

#include "functions.h"

#include <const.h>


std::wstring ConvertANSIToUNICODE(const char* source, const unsigned int length)
{
	if (source == nullptr || length < 1) // Fail
		return std::wstring();

	int len = MultiByteToWideChar(CP_ACP, 0, source, length, 0, 0) - 1;

	std::wstring r(len, '\0');

	MultiByteToWideChar(CP_ACP, 0, source, length, &r[0], len);

	return r;
}