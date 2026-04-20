#pragma once

#include "Graphics.h"

namespace CubeRenderer {
	class BitmapGraphics : public Graphics
	{
		void CreateD2DDeviceAndContext();

		ID3D11Texture2D* RenderToTexture(float angle, float x, float y, float z);

		ID2D1Bitmap1* RenderToBitmap();

		void Resize(UINT width, UINT height);

		void SaveBitmapToFile(ID2D1Bitmap* bitmap, const WCHAR* fileName);
		void SaveTextureToFIle(ID3D11Texture2D* texture, WCHAR* fileName);

		HRESULT CreateD2DBitmapFromD3DTexture(
			ID3D11Texture2D* d3dTexture,
			ID2D1DeviceContext* d2dContext,
			ID2D1Bitmap1** outD2dBitmap);
		ID2D1Bitmap1* CreateD2DBitmapFromTexture(ID3D11Texture2D* texture);

		//ID3D11Texture2D* RenderToTexture(float angle, float x, float y, float z);


	private:
		ComPtr<ID2D1Device> d2dDevice;
		ComPtr<ID2D1DeviceContext> d2dContext;

		ID2D1Bitmap1* d2dTargetBitmap1;


		inline void ThrowIfFailed(HRESULT hr)
		{
			if (FAILED(hr))
			{
				if (OnError) OnError(hr);
				else throw runtime_error("HRESULT failed: " + to_string(hr));
			}
		}
	};
}

