#pragma once

//#include <functional>

#include "Base.h"

#include "Texture.h"
//#include <Windows.h>

namespace CubeRenderer {
	struct Vertex {
		struct { float x, y, z; } position;
		struct { float u, v; } textureCoordinate;
	};


	class Scene
	{
	private:
		size_t verticesSize = 0;
		Vertex* vertices;

		UINT indexCount;
		size_t indicesSize = 0;
		USHORT* indices;

		UINT cubeCount = 0;

		Texture* texture;

		Vertex* CreateVertices(float width, float height, float depth, float offsetX, float offsetY, float offsetZ, float u, float v, Texture* texture, size_t* sizeOut, UINT* countOut);
		USHORT* CreateIndices(USHORT offset, size_t* sizeOut, UINT* countOut);

		inline void ThrowIfFailed(HRESULT hr)
		{
			if (FAILED(hr))
			{
				if (OnError) OnError(hr);
			}
		}
	public:
		Scene();
		UINT GetIndexCount();
		size_t GetIndicesSize();
		size_t GetVerticesSize();
		Vertex* GetVertices();
		USHORT* GetIndices();
		void AddCube(float width, float height, float depth, float x, float y, float z, float u, float v, Texture* texture);

		void SetTexture(Texture* texture);
		Texture* GetTexture();

		std::function<void(HRESULT)> OnError;
	};
}
