# pragma once

// pre-compiled header <pch.h>

// link library dependencies

#pragma comment(lib,"Shlwapi.lib")

// includes
#include <any>
#include <array>
#include <algorithm>
#include <execution>
// TODO: #include <format> C++ 20
#include <functional>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <optional>
// TODO: #include <ranges> C++ 20
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
#include <XAudio2.h>

#include <wrl/client.h>
#include <comdef.h>
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
#include <vorbis/codec.h>		// libvorbis:x64-windows           
#include <vorbis/vorbisfile.h>	// libvorbis:x64-windows
#include <fmt/core.h> // fmt:x64-windows
#include <fmt/format.h> // fmt:x64-windows

#include <Dependencies/LuaPlus/LuaPlus/LuaPlus.h>  // TODO: learn how include the build of this... (there are no headers)