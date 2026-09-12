#include "pch.h"
#include "include/Plane.h"

namespace CubeRenderer {

	Plane CreatePlane(float width, float height) {
		Plane plane;

		float halfWidth = width / 2.0f;
		float halfHeight = height / 2.0f;

		Vertex vertices[] = {
			{ { -halfWidth, -halfHeight, 0.0f }, { 0.0f, 1.0f } },
			{ {  halfWidth, -halfHeight, 0.0f }, { 1.0f, 1.0f } },
			{ {  halfWidth,  halfHeight, 0.0f }, { 1.0f, 0.0f } },
			{ { -halfWidth,  halfHeight, 0.0f }, { 0.0f, 0.0f } },
		};
		USHORT indices[] = { 0, 1, 2, 0, 2, 3 };

		plane.vertexCount = sizeof(vertices) / sizeof(Vertex);
		plane.indexCount = sizeof(indices) / sizeof(USHORT);

		plane.vertices = (Vertex*)malloc(sizeof(vertices));
		plane.indices = (USHORT*)malloc(sizeof(indices));

		if (plane.vertices) memcpy(plane.vertices, vertices, sizeof(vertices));
		if (plane.indices) memcpy(plane.indices, indices, sizeof(indices));

		return plane;
	}

	void DeletePlane(Plane& plane) {
		free(plane.vertices);
		free(plane.indices);
		plane.vertices = nullptr;
		plane.indices = nullptr;
		plane.vertexCount = 0;
		plane.indexCount = 0;
	}
}