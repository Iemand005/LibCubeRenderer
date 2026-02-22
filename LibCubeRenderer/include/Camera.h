#pragma once

#include <DirectXMath.h>
#include <DirectXPackedVector.h>

class Camera
{
public:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;

	Camera() {
	}
};

