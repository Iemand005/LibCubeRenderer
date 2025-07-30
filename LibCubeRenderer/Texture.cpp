#include "pch.h"
#include "include/Texture.h"

namespace CubeRenderer {

	UINT Texture::GetWidth() {
		UINT width;
		GetDimensions(&width, NULL);
		return width;
	}

	UINT Texture::GetHeight() {
		UINT height;
		GetDimensions(NULL, &height);
		return height;
	}

	void Texture::GetDimensions(PUINT width, PUINT height) {
		D3D11_TEXTURE2D_DESC textureDesc;
		texture->GetDesc(&textureDesc);
		if (width) *width = textureDesc.Width;
		if (height) *height = textureDesc.Height;
	}
}