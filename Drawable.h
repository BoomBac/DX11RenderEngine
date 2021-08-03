#pragma once
#include <memory>
#include <vector>
#include "IndexBuffer.h"
#include <DirectXMath.h>
#include "vector3D.h"

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
	//返回mvp变换矩阵
	WorldTransform& GetTransform();
	//更新摄像机变换
	static void UpdateCameraTransformation(const DirectX::XMMATRIX& tranf);
	virtual void SetActorLocation(const CusMath::vector3d& t);
	virtual void SetActorRotation(const CusMath::vector3d& r);
	virtual void SetActorScale(const CusMath::vector3d& s);
protected:
	std::vector<std::unique_ptr<Bindable>> binds;
	IndexBuffer* indexbuffer;
	//mvp变换矩阵，所有Drawable共享一份view和projection
	WorldTransform transform;
	static DirectX::XMMATRIX view;
	static DirectX::XMMATRIX projection;
	//保存变换信息
	CusMath::vector3d Location;
	CusMath::vector3d Rotation;
	CusMath::vector3d Scale;
	//保存自创建以来，所有变换数据 0t 1r 2s
	DirectX::XMMATRIX DonedTransforms[3] = { DirectX::XMMatrixIdentity(),DirectX::XMMatrixIdentity(),
		DirectX::XMMatrixIdentity()};
private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const=0;
};

