#pragma once

#include <QDebug>
#include "Public\Tool\Utili.h"


template<class T>  //每个模板子类静态成员不共享
class Shape :
	public Drawable
{

public:
	bool isInitialzed() const;
	void AddStaticBind(std::unique_ptr<BindableInterface> bind);
	void AddStaticIndexBuf(std::unique_ptr<IndexBuffer> ibf, Graphics& gfx);
	//更新常量缓冲区中的世界变换
	virtual  ~Shape();
	virtual void SetWorldLocation(const CusMath::vector3d& t) override;
	virtual void AddWorldLocation(const CusMath::vector3d& t) override;
	virtual void SetActorLocation(const CusMath::vector3d& t) override;
	virtual void AddActorLocation(const CusMath::vector3d& t) override;
	virtual void AddWorldRotation(const CusMath::vector3d& r) override;
	virtual void SetWorldRotation(const CusMath::vector3d& r) override;
	virtual void AddActorRotation(const CusMath::vector3d& r) override;
	virtual void SetActorRotation(const CusMath::vector3d& r) override;
	virtual void SetActorScale(const CusMath::vector3d& s) override;
	virtual void AdjustActorRotation(float angle, EAxisType axis);
	virtual void AdjustWorldRotation(float angle, EAxisType axis);
private:
	static std::vector<std::unique_ptr<BindableInterface>> StaticBinds;
private:
	virtual const std::vector<std::unique_ptr<BindableInterface>>& GetStaticBinds() const override
	{
		return StaticBinds;
	}
	virtual void Update(const DirectX::XMMATRIX& transf=DirectX::XMMatrixIdentity()) override;
	void UpdateOrient();
	//transf为将要变化的旋转值，传入角度

	//void AdjustRotation(const CusMath::vector3d& transf, const CusMath::vector3d& coordinate, DirectX::XMMATRIX* target_martix);
protected:
	void SetIndexbufferFromSBinds();
};

template<class T>
Shape<T>::~Shape()
{

}


template<class T>
void Shape<T>::UpdateOrient()
{
	forward_direction_ = kDefaultForwardDirection;
	right_direction_ = kDefaultRightDirection;
	up_direction_ = kDefaultUpDirection;
	DirectX::XMStoreFloat3(&forward_direction_,
		DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&forward_direction_),
			DonedTransforms[1]));
	DirectX::XMStoreFloat3(&right_direction_,
		DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&right_direction_),
			DonedTransforms[1]));
	DirectX::XMStoreFloat3(&up_direction_,
		DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&up_direction_),
			DonedTransforms[1]));
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
	world_location_.x += t.z * forward_direction_.x + t.x * right_direction_.x
		+ t.y * up_direction_.x;
	world_location_.y += t.z * forward_direction_.y + t.x  * right_direction_.y
		+ t.y * up_direction_.y;
	world_location_.z += t.z  * forward_direction_.z + t.x  * right_direction_.z
		+ t.y * up_direction_.z;
	Drawable::AddActorLocation(t);
	DonedTransforms[0] = DirectX::XMMatrixTranslation(world_location_.x, world_location_.y, world_location_.z);
}

template<class T>
void Shape<T>::SetActorLocation(const CusMath::vector3d& t)
{
	world_location_.x += (t.z - object_location_.z) * forward_direction_.x + (t.x - object_location_.x) * right_direction_.x
		+ (t.y - object_location_.y) * up_direction_.x;
	world_location_.y += (t.z - object_location_.z) * forward_direction_.y + (t.x - object_location_.x) * right_direction_.y
		+ (t.y - object_location_.y) * up_direction_.y;
	world_location_.z += (t.z - object_location_.z) * forward_direction_.z + (t.x - object_location_.x) * right_direction_.z
		+ (t.y - object_location_.y) * up_direction_.z;
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
void Shape<T>::AdjustActorRotation(float angle, EAxisType axis)
{
	DirectX::XMVECTOR Q;
	if (axis == EAxisType::kXAxis)
	{
		Q = DirectX::XMQuaternionRotationAxis(kAxisX, DegToRad(angle));
	}
	else if (axis == EAxisType::kYAxis)
	{
		Q = DirectX::XMQuaternionRotationAxis(kAxisY, DegToRad(angle));
	}
	else if (axis == EAxisType::kZAxis)
	{
		Q = DirectX::XMQuaternionRotationAxis(kAxisZ, DegToRad(angle));
	}
	DonedTransforms[1] = DirectX::XMMatrixRotationQuaternion(Q) * DonedTransforms[1];
}

template<class T>
void Shape<T>::AdjustWorldRotation(float angle, EAxisType axis)
{

	DirectX::XMVECTOR Q;
	if (axis == EAxisType::kXAxis)
	{
		Q = DirectX::XMQuaternionRotationAxis(kAxisX, DegToRad(angle));
		quat_rotatioin_x = DirectX::XMQuaternionMultiply(quat_rotatioin_x, Q);
	}
	else if (axis == EAxisType::kYAxis)
	{
		Q = DirectX::XMQuaternionRotationAxis(kAxisY, DegToRad(angle));
		quat_rotatioin_y = DirectX::XMQuaternionMultiply(quat_rotatioin_y, Q);
	}
	else if (axis == EAxisType::kZAxis)
	{
		Q = DirectX::XMQuaternionRotationAxis(kAxisZ, DegToRad(angle));
		quat_rotatioin_z = DirectX::XMQuaternionMultiply(quat_rotatioin_z, Q);
	}
	//qDebug() << "x rotation:" << RadToDeg(DirectX::XMScalarACos(quat_rotatioin_x.m128_f32[3]) * 2.f);
	//qDebug() << "y rotation:" << RadToDeg(DirectX::XMScalarACos(quat_rotatioin_y.m128_f32[3]) * 2.f);
	//qDebug() << "z rotation:" << RadToDeg(DirectX::XMScalarACos(quat_rotatioin_z.m128_f32[3]) * 2.f);
	DonedTransforms[1] *= DirectX::XMMatrixRotationQuaternion(Q);
}

template<class T>
void Shape<T>::AddWorldRotation(const CusMath::vector3d& r)
{
	Drawable::AddWorldRotation(r);
	AdjustWorldRotation(r.x, EAxisType::kXAxis);
	AdjustActorRotation(r.y, EAxisType::kYAxis);
	AdjustWorldRotation(r.z, EAxisType::kZAxis);
	UpdateOrient();
}

//qDebug() <<"x:" << world_rotation_.x <<"y:" << world_rotation_.y << "z:"<<world_rotation_.z;
template<class T>
void Shape<T>::SetWorldRotation(const CusMath::vector3d& r)
{
	auto tr = world_rotation_ - r;
	Drawable::SetWorldRotation(r);
	AdjustWorldRotation(tr.x, EAxisType::kXAxis);
	AdjustWorldRotation(tr.y, EAxisType::kYAxis);
	AdjustWorldRotation(tr.z, EAxisType::kZAxis);
	UpdateOrient();
}


template<class T>
void Shape<T>::AddActorRotation(const CusMath::vector3d& r)
{
	//TODO() : 自身旋转也会对世界旋转造成影响。暂时未加入
	Drawable::AddActorRotation(r);
	AdjustActorRotation(r.x, EAxisType::kXAxis);
	AdjustActorRotation(r.y, EAxisType::kYAxis);
	AdjustActorRotation(r.z, EAxisType::kZAxis);
	UpdateOrient();
}

template<class T>
void Shape<T>::SetActorRotation(const CusMath::vector3d& r)
{
	auto tr = object_rotation_ - r;
	Drawable::SetActorRotation(r);
	AdjustActorRotation(tr.x, EAxisType::kXAxis);
	AdjustActorRotation(tr.y, EAxisType::kYAxis);
	AdjustActorRotation(tr.z, EAxisType::kZAxis);
	UpdateOrient();
}


template<class T>
void Shape<T>::Update(const DirectX::XMMATRIX& transf/*=DirectX::XMMatrixIdentity()*/)
{
	//先平移。如果先缩放，那么平移的轴距就会被缩放改变。如果先旋转，那么绕三个轴旋转就会变成绕世界原点的旋转
	// 自身旋转 * 世界旋转 * 缩放 * 平移
	v_cons_buf_.mvp_matrix_ = DonedTransforms[1] * DonedTransforms[2] * DonedTransforms[0] * view * projection;
	v_cons_buf_.world_matrix_ = DonedTransforms[1] * DonedTransforms[2] * DonedTransforms[0];
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

