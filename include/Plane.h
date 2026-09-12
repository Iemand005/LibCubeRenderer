#pragma once

#include "Base.h"
#include "Scene.h"

namespace CubeRenderer {

	struct Plane {
		UINT vertexCount = 0;
		UINT indexCount = 0;
		Vertex* vertices = nullptr;
		USHORT* indices = nullptr;
	};

	// Generates a textured quad in the XY plane, centered on the origin.
	// Caller owns the buffers and must call DeletePlane().
	Plane CreatePlane(float width, float height);

	void DeletePlane(Plane& plane);
}