#pragma once
#include <memory>
#include <vector>
#include "IndexBuffer.h"
#include <DirectXMath.h>

class Graphics;
class Bindable;

struct WorldTransform
{
	DirectX::XMMATRIX mWorld; //4x4
};

class Drawable
{
	template<class T>
	friend class Shape;
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	void Draw(Graphics& gfx);
	void AddBind(std::unique_ptr<Bindable> bind);
	void AddIndexBuf(std::unique_ptr<IndexBuffer> ibf, Graphics& gfx);
	~Drawable() = default;
	//作为可绘制对象的基类，定义了一个mvp矩阵，其中m可由子类更改
	WorldTransform& GetTransform();
	//更新摄像机变换
	static void UpdateCameraTransformation(const DirectX::XMMATRIX& tranf);
protected:
	std::vector<std::unique_ptr<Bindable>> binds;
	IndexBuffer* indexbuffer;
	WorldTransform transform;
	static DirectX::XMMATRIX view;
	static DirectX::XMMATRIX projection;
private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const=0;
};

