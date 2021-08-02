#pragma once
#include "GraphicsResource.h"
#include <d3d11.h>
#include <string>

enum class EBindableType
{
	VertexBuffer,
	IndexBuffer,
	InputLayout,
	VertexShader,
	PixelShader,
	ConstantBuffer
};
class Bindable : public GraphicsResource
{
public:
	~Bindable() = default;
	virtual void Bind(Graphics& gfx) = 0;
	virtual EBindableType GetType() const=0;
protected:
};

