#pragma once

#include "Base.h"

namespace CubeRenderer {
	using namespace Microsoft::WRL;

	class Texture {
	public:

		Texture() = default;
		Texture(PVOID pixels, UINT width, UINT height);

		UINT GetWidth();
		UINT GetHeight();
		void GetDimensions(PUINT width, PUINT height);
	private:
		PBYTE pixels;
		ComPtr<ID3D11Texture2D> texture;
	};
}