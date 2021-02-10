# pragma once

// pre-compiled header <pch.h>

// link library dependencies

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"Shlwapi.lib")

// includes
#include <algorithm>
// TODO: #include <format>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <string>
#include <memory>
#include <vector>


// Windows headers
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

/*
	Third Party libraries

	Installed with vcpkg for x64-windows
	ref: https://docs.microsoft.com/en-us/cpp/build/vcpkg

*/
#include <tinyxml2.h>
#include <zlib.h>  // C:\tools\vcpkg\vcpkg.exe install zlib:x64-windows