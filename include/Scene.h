#pragma once

//#include <functional>

//#include <Windows.h>

#include "Base.h"

#include "Texture.h"

//#include <Windows.h>

namespace CubeRenderer {
struct Vertex {
	 struct { float x, y, z; } position;
	 struct { float u, v; } textureCoordinate;
	};