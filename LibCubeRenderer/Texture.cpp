#include "pch.h"
#include "include/Texture.h"

namespace CubeRenderer {
	Texture::Texture(PVOID pixels, UINT width, UINT height)
	{
		UINT bufferSize = width * height * 4;

		this->pixels = (PBYTE)malloc(bufferSize);
		memcpy(this->pixels, pixels, bufferSize);

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pSysMem = this->pixels;
		subresourceData.SysMemPitch = width * 4;
		subresourceData.SysMemSlicePitch = 0;

		ID3D11Texture2D* pTexture = nullptr;
		ThrowIfFailed(device->CreateTexture2D(&textureDesc, &subresourceData, &pTexture));
	}

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