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

	}

	ULONG  STDMETHODCALLTYPE Release() override {

	}
	HRESULT STDMETHODCALLTYPE QueryInterface(
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override {
	};

	ULONG STDMETHODCALLTYPE AddRef(void) override {

	};

};

