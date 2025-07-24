#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <wrl/client.h>

#include "include/Base.h"

#include <d3d11.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#include <gdiplus.h>

#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "gdiplus.lib")

#include "GraphicsPrivate.h"