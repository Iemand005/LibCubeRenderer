#pragma once

#include "Base.h"

namespace CubeRenderer {
	using namespace Microsoft::WRL;

	class Texture {
	public:

		Texture() = default;
		Texture(ID3D11Texture2D* texture);

		UINT GetWidth();
		UINT GetHeight();
		void GetDimensions(PUINT width, PUINT height);

		ID3D11Texture2D* GetResource();

	private:
		ComPtr<ID3D11Texture2D> texture;
	};
}