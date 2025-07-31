#pragma once

#include <functional>
#include <filesystem>
#include <string>

#include <wrl/client.h>

#include <dxgi1_6.h>
#include <d2d1_1.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include <gdiplus.h>
#include <wincodec.h>

#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "windowscodecs.lib")

#include <Windows.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
