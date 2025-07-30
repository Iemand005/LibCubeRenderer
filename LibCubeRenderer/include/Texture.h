#pragma once

namespace CubeRenderer {
	class Texture {
	public:
		UINT GetWidth();
		UINT GetHeight();
	private:
		UINT width;
		UINT height;
	};
}