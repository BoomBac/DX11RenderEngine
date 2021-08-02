#pragma once
#include <memory>
#include <vector>
#include "IndexBuffer.h"
#include <DirectXMath.h>

class Graphics;
class Bindable;

struct MVPTransform
{
	DirectX::XMMATRIX mWorld; //4x4
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
};

class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	void Draw(Graphics& gfx);
	void AddBind(std::unique_ptr<Bindable> bind);
	void AddIndexBuf(std::unique_ptr<IndexBuffer> ibf, Graphics& gfx);
	~Drawable() = default;
	MVPTransform& GetTransform();
protected:
	std::vector<std::unique_ptr<Bindable>> binds;
	std::unique_ptr<IndexBuffer> indexbuffer;
	MVPTransform transform;
};

