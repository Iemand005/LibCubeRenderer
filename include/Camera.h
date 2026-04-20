#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

class Camera : public IUnknown
{
public:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;

	Camera() {
		position = { 0, 0, 0 };
		rotation = { 0, 0, 0 };
	}

	ULONG  STDMETHODCALLTYPE Release() override {
		return 0;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override {
		return S_OK;
	}

	ULONG STDMETHODCALLTYPE AddRef(void) override {
		return 0;
	}

};

