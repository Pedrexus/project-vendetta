# pragma once

// pre-compiled header <pch.h>

// link library dependencies

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"Shlwapi.lib")

// includes
#include <any>
#include <algorithm>
#include <execution>
// TODO: #include <format>
#include <functional>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <ranges>
#include <set>
#include <string>
#include <vector>

#include <iostream>
#include <fstream>

#include <cmath>

// Windows headers
#include <Windows.h>
#include <Shlobj.h>
#include <Knownfolders.h>

// DirectX Headers
#include <d3d11_4.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include <wrl/client.h>
#include <crtdbg.h>
#include <tchar.h>
#include <direct.h>
#include <shlwapi.h>
#include <concurrent_queue.h>

/*
	Third Party libraries

	Installed with vcpkg for x64-windows
	ref: https://docs.microsoft.com/en-us/cpp/build/vcpkg

	>>> C:\tools\vcpkg\vcpkg.exe search tinyxml
	>>> C:\tools\vcpkg\vcpkg.exe install tinyxml2:x64-windows
*/
#include <tinyxml2.h>
#include <zlib.h>  // C:\tools\vcpkg\vcpkg.exe install zlib:x64-windows

#include <Dependencies/LuaPlus/LuaPlus/LuaPlus.h>  // TODO: learn how include the build of this... (there are no headers)