#pragma once

#ifdef MYSTATICLIB_EXPORTS
#define CRLIB_API __declspec(dllexport)  // When compiling the LIB
#else
#define CRLIB_API __declspec(dllimport)  // When compiling the APP
#endif

#include <functional>
#include <filesystem>

#include <d3d11.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#include <gdiplus.h>

#include <dwmapi.h>

#include <wrl/client.h>

#include <Windows.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "gdiplus.lib")