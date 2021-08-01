#pragma once
#include "Graphics.h"
#include <d3d11.h>

class Graphics;
class GraphicsResource
{
protected:
	static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
	static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
};