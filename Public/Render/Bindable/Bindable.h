#pragma once
#include <d3d11.h>
#include <string>
#include "..\GraphicsResource.h"

enum class EBindableType
{
	VertexBuffer,
	IndexBuffer,
	InputLayout,
	VertexShader,
	PixelShader,
	ConstantBuffer,
	DepthStencilBuffer
};
class Bindable : public GraphicsResource
{
public:
	~Bindable() = default;
	virtual void Bind(Graphics& gfx) = 0;
	virtual EBindableType GetType() const=0;
protected:
};

