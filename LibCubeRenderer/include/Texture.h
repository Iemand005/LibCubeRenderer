#pragma once

#include "Base.h"

namespace CubeRenderer {
	using namespace Microsoft::WRL;

	class Texture {
	public:
		UINT GetWidth();
		UINT GetHeight();
		void GetDimensions(PUINT width, PUINT height);
	private:
		ComPtr<ID3D11Texture2D> texture;
	};
}