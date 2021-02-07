# pragma once

// pre-compiled header <pch.h>

// link library dependencies

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"Shlwapi.lib")

// includes

#include <stdexcept>

#include <Windows.h>
#include <Shlobj.h>
#include <Knownfolders.h>
#include <DirectXMath.h>

#include <wrl/client.h>
#include <d3d11_4.h>
#include <crtdbg.h>
#include <tchar.h>
#include <direct.h>
#include <shlwapi.h>

#include <memory>
#include <map>
#include <functional>

/*
	Third Party libraries

	Installed with vcpkg for x64-windows
	ref: https://docs.microsoft.com/en-us/cpp/build/vcpkg

*/
#include <tinyxml2.h>

// project headers
#include <const.h>
#include <macros.h>

#include <ApplicationLayer/GameApp.h>
#include <ApplicationLayer/Initialization.h>
#include <ApplicationLayer/Actors.h>