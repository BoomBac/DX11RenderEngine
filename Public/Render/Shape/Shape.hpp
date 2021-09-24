#pragma once

#include <QDebug>
#include "Public\Tool\Utili.h"

namespace
{
	constexpr DirectX::XMFLOAT3 g_kDefaultForwardDirection(0.f, 0.f, 1.f);
	constexpr DirectX::XMFLOAT3 g_kDefaultRightDirection(1.f, 0.f, 0.f);
	constexpr DirectX::XMFLOAT3 g_kDefaultUpDirection(0.f, 1.f, 0.f);
}


template<class T>  //每个模板子类静态成员不共享
class Shape :
	public Drawable
{

public:
	bool isInitialzed() const;
	void AddStaticBind(std::unique_ptr<BindableInterface> bind);
	void AddStaticIndexBuf(std::unique_ptr<IndexBuffer> ibf, Graphics& gfx);
	//更新常量缓冲区中的世界变换
	virtual void SetWorldLocation(const CusMath::vector3d& t) override;
	virtual void AddWorldLocation(const CusMath::vector3d& t) override;
	virtual void SetActorLocation(const CusMath::vector3d& t) override;
	virtual void AddActorLocation(const CusMath::vector3d& t) override;
	virtual void AddWorldRotation(const CusMath::vector3d& r) override;
	virtual void SetWorldRotation(const CusMath::vector3d& r) override;
	virtual void AddActorRotation(const CusMath::vector3d& r) override;
	virtual void SetActorRotation(const CusMath::vector3d& r) override;
	virtual void SetActorScale(const CusMath::vector3d& s) override;
private:
	static std::vector<std::unique_ptr<BindableInterface>> StaticBinds;
private:
	virtual const std::vector<std::unique_ptr<BindableInterface>>& GetStaticBinds() const override
	{
		return StaticBinds;
	}
	virtual void Update(const DirectX::XMMATRIX& transf=DirectX::XMMatrixIdentity()) override;
	void UpdateOrient();
protected:
	void SetIndexbufferFromSBinds();
};

template<class T>
void Shape<T>::AddWorldRotation(const CusMath::vector3d& r)
{
	Drawable::AddWorldRotation({ DegToRad(r.x), DegToRad(r.y), DegToRad(r.z) });
	//更新物体朝向
	DonedTransforms[1] = DirectX::XMMatrixRotationRollPitchYaw(world_rotation_.x, world_rotation_.y, world_rotation_.z);
	Update();
}

template<class T>
void Shape<T>::UpdateOrient()
{
	forward_direction = g_kDefaultForwardDirection;
	right_direction = g_kDefaultRightDirection;
	up_direction = g_kDefaultUpDirection;
	DirectX::XMStoreFloat3(&forward_direction,
		DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&forward_direction),
			DonedTransforms[1] * DonedTransforms[3]));
	DirectX::XMStoreFloat3(&right_direction,
		DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&right_direction),
			DonedTransforms[1] * DonedTransforms[3]));
	DirectX::XMStoreFloat3(&up_direction,
		DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&up_direction),
			DonedTransforms[1] * DonedTransforms[3]));
}

template<class T>
void Shape<T>::AddWorldLocation(const CusMath::vector3d& t)
{
	//重置物体自身变换
	//object_location_ = CusMath::vector3d(0.f, 0.f, 0.f);
	Drawable::AddWorldLocation(t);
	DonedTransforms[0] = DirectX::XMMatrixTranslation(world_location_.x, world_location_.y, world_location_.z);
	Update();
}

template<class T>
void Shape<T>::SetWorldLocation(const CusMath::vector3d& t)
{
	//重置物体自身变换
	object_location_ = CusMath::vector3d(0.f, 0.f, 0.f);
	Drawable::SetWorldLocation(t);
	DonedTransforms[0] = DirectX::XMMatrixTranslation(world_location_.x, world_location_.y, world_location_.z);
}

template<class T>
void Shape<T>::AddActorLocation(const CusMath::vector3d& t)
{
	world_location_.x += t.z * forward_direction.x + t.x * right_direction.x
		+ t.y * up_direction.x;
	world_location_.y += t.z * forward_direction.y + t.x  * right_direction.y
		+ t.y * up_direction.y;
	world_location_.z += t.z  * forward_direction.z + t.x  * right_direction.z
		+ t.y * up_direction.z;
	Drawable::AddActorLocation(t);
	DonedTransforms[0] = DirectX::XMMatrixTranslation(world_location_.x, world_location_.y, world_location_.z);
}

template<class T>
void Shape<T>::SetActorLocation(const CusMath::vector3d& t)
{
	world_location_.x += (t.z - object_location_.z) * forward_direction.x + (t.x - object_location_.x) * right_direction.x
		+ (t.y - object_location_.y) * up_direction.x;
	world_location_.y += (t.z - object_location_.z) * forward_direction.y + (t.x - object_location_.x) * right_direction.y
		+ (t.y - object_location_.y) * up_direction.y;
	world_location_.z += (t.z - object_location_.z) * forward_direction.z + (t.x - object_location_.x) * right_direction.z
		+ (t.y - object_location_.y) * up_direction.z;
	Drawable::SetActorLocation(t);
	DonedTransforms[0]= DirectX::XMMatrixTranslation(world_location_.x,world_location_.y,world_location_.z);
	Update();
}


template<class T>
void Shape<T>::SetActorScale(const CusMath::vector3d& s)
{
	Drawable::SetActorScale(s);
	DonedTransforms[2] = DirectX::XMMatrixScaling(s.x, s.y, s.z);
	Update();
}


template<class T>
void Shape<T>::SetWorldRotation(const CusMath::vector3d& r)
{
	Drawable::SetWorldRotation(r);
	//先进行新的变化，然后再将老变化加入
	//就是将上次变换在本次变换后的坐标系进行变换 本次要影响上次变换
	if (r.x == world_rotation_.x && r.y == world_rotation_.y)
	{
		DonedTransforms[3] = DirectX::XMMatrixRotationX(DegToRad(world_rotation_.x)) *
			DirectX::XMMatrixRotationY(DegToRad(world_rotation_.y)) *
			DirectX::XMMatrixRotationZ(DegToRad(world_rotation_.z));
	}
	if (r.x == world_rotation_.x && r.z == world_rotation_.z)
	{
		DonedTransforms[3] = DirectX::XMMatrixRotationZ(DegToRad(world_rotation_.z)) *
			DirectX::XMMatrixRotationX(DegToRad(world_rotation_.x)) *
			DirectX::XMMatrixRotationY(DegToRad(world_rotation_.y));
	}
	if (r.y == world_rotation_.y && r.z == world_rotation_.z)
	{
		DonedTransforms[3] = DirectX::XMMatrixRotationZ(DegToRad(world_rotation_.z)) *
			DirectX::XMMatrixRotationY(DegToRad(world_rotation_.y)) *
			DirectX::XMMatrixRotationX(DegToRad(world_rotation_.x));
	}
	UpdateOrient();
}


template<class T>
void Shape<T>::AddActorRotation(const CusMath::vector3d& r)
{

}

template<class T>
void Shape<T>::SetActorRotation(const CusMath::vector3d& r)
{
	Drawable::SetActorRotation(r);
	//先进行新的变化，然后再将老变化加入
	//就是将上次变换在本次变换后的坐标系进行变换 本次要影响上次变换
	if (r.x == object_rotation_.x && r.y == object_rotation_.y)
	{
		DonedTransforms[1] = DirectX::XMMatrixRotationX(DegToRad(object_rotation_.x)) *
			DirectX::XMMatrixRotationY(DegToRad(object_rotation_.y)) *
			DirectX::XMMatrixRotationZ(DegToRad(object_rotation_.z));
	}
	if (r.x == object_rotation_.x && r.z == object_rotation_.z)
	{
		DonedTransforms[1] = DirectX::XMMatrixRotationZ(DegToRad(object_rotation_.z)) *
			DirectX::XMMatrixRotationX(DegToRad(object_rotation_.x)) *
			DirectX::XMMatrixRotationY(DegToRad(object_rotation_.y));			
	}
	if (r.y == object_rotation_.y && r.z == object_rotation_.z)
	{
		DonedTransforms[1] = DirectX::XMMatrixRotationZ(DegToRad(object_rotation_.z)) *
			DirectX::XMMatrixRotationY(DegToRad(object_rotation_.y))*
			DirectX::XMMatrixRotationX(DegToRad(object_rotation_.x));
	}
	UpdateOrient();
}


template<class T>
void Shape<T>::Update(const DirectX::XMMATRIX& transf/*=DirectX::XMMatrixIdentity()*/)
{
	//先平移。如果先缩放，那么平移的轴距就会被缩放改变。如果先旋转，那么绕三个轴旋转就会变成绕世界原点的旋转
	// 自身旋转 * 世界旋转 * 缩放 * 平移
	transform.mWorld = DonedTransforms[1] * DonedTransforms[3] * DonedTransforms[2] * DonedTransforms[0] * view * projection;
}

template<class T>
void Shape<T>::SetIndexbufferFromSBinds()
{
	for (auto& i : StaticBinds)
	{
		if (i->GetType() == EBindableType::kIndexBuffer)
		{
			indexbuffer = dynamic_cast<IndexBuffer*>(i.get());
		}
	}
}

template<class T>
std::vector<std::unique_ptr<BindableInterface>> Shape<T>::StaticBinds;

template<class T>
bool Shape<T>::isInitialzed() const
{
	return !StaticBinds.empty();
}
template<class T>
void Shape<T>::AddStaticBind(std::unique_ptr<BindableInterface> bind)
{
	StaticBinds.push_back(std::move(bind));
}
template<class T>
void Shape<T>::AddStaticIndexBuf(std::unique_ptr<IndexBuffer> ibf, Graphics& gfx)
{
	indexbuffer = ibf.get();
	StaticBinds.push_back(std::move(ibf));
}

