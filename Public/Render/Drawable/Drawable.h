#ifndef DX11ENGINE_RENDER_DRAWABLE_DRAWABLE_H
#define DX11ENGINE_RENDER_DRAWABLE_DRAWABLE_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

#include <memory>
#include <vector>
#include <DirectXMath.h>

#include "Public/Render/Bindable/IndexBuffer.h"
#include "Public/Render/Graphics.h"
#include "Public/Render/Bindable/BindableInterface.h"

// mvp在一起
struct WorldTransform
{
	DirectX::XMMATRIX mWorld; //4x4
};
constexpr int kDefaultForwardDirection = 2;

class Drawable
{
	template<class T>
	friend class Shape;
public:

	Drawable() = default;
	virtual void Draw(Graphics& gfx);
	void AddBind(std::unique_ptr<BindableInterface> bind);
	void AddIndexBuf(std::unique_ptr<IndexBuffer> ibf, Graphics& gfx);
	~Drawable() = default;
	//返回mvp变换矩阵，对于每一个物体而言，vp都是共享一份的
	WorldTransform& GetTransform();
	//设置物体变换
	virtual void SetWorldLocation(const CusMath::vector3d& t);
	virtual void AddWorldLocation(const CusMath::vector3d& t);
	virtual void SetActorLocation(const CusMath::vector3d& t);
	virtual void AddActorLocation(const CusMath::vector3d& t);
	virtual void SetWorldRotation(const CusMath::vector3d& r);
	virtual void AddWorldRotation(const CusMath::vector3d& r);
	virtual void AddActorRotation(const CusMath::vector3d& r);
	virtual void SetActorRotation(const CusMath::vector3d& r);
	virtual void SetActorScale(const CusMath::vector3d& s);

	const CusMath::vector3d GetActorLocation() const;
	const CusMath::vector3d GetWorldLocation() const;
	const CusMath::vector3d GetActorRotation() const;
	const CusMath::vector3d GetWorldRotation() const;
	//物体自身坐标轴
	DirectX::XMFLOAT3 forward_direction = DirectX::XMFLOAT3(0.f,0.f,1.f);
	DirectX::XMFLOAT3 right_direction = DirectX::XMFLOAT3(1.f, 0.f, 0.f);
	DirectX::XMFLOAT3 up_direction = DirectX::XMFLOAT3(0.f, 1.f, 0.f);

	DirectX::XMMATRIX GetTranslateMartix() const;
	DirectX::XMMATRIX GetRotationMartix() const;

protected:
	std::vector<std::unique_ptr<BindableInterface>> binds;
	IndexBuffer* indexbuffer;
	//mvp变换矩阵，所有Drawable共享一份view和projection
	WorldTransform transform;
	//test
	static DirectX::XMMATRIX view;
	static DirectX::XMMATRIX projection;
	//保存世界变换信息
	CusMath::vector3d world_location_;
	CusMath::vector3d world_rotation_;
	//保存自身变换信息
	CusMath::vector3d object_location_ = {0.f,0.f,0.f};
	CusMath::vector3d object_rotation_ = {0.f,0.f,0.f};
	CusMath::vector3d Scale;
	//保存自创建以来，所有变换数据
	//0:word_translate 1:world_rotation 2:scale 3:local_rotation
	DirectX::XMMATRIX DonedTransforms[4] = { DirectX::XMMatrixIdentity(),DirectX::XMMatrixIdentity(),
		DirectX::XMMatrixIdentity(),DirectX::XMMatrixIdentity()};
	//更新物体变换
	virtual void Update(const DirectX::XMMATRIX& transf = DirectX::XMMatrixIdentity()) {};
private:
	DISALLOW_COPY_AND_ASSIGN(Drawable)
	virtual const std::vector<std::unique_ptr<BindableInterface>>& GetStaticBinds() const=0;
};

#endif //DX11ENGINE_RENDER_DRAWABLE_DRAWABLE_H