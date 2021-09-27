#include "Public/Render/Drawable/Drawable.h"
#include "Public/Tool/Utili.h"
#include "Public/Render/Graphics.h"
#include "Public/Render/Bindable/BindableInterface.h"
#include "Public/Render/Bindable/IndexBuffer.h"


namespace dx = DirectX;

DirectX::XMMATRIX Drawable::view;
DirectX::XMMATRIX Drawable::projection;

void Drawable::Draw(Graphics& gfx)
{
	if (!visiblity_) return;
	view = gfx.camera_.view_matrix();
	for (auto& i : binds)
	{
		i->Bind(gfx);
	}
	for (auto& i : GetStaticBinds())
	{
		i->Bind(gfx);
	}
	Update();
	gfx.DrawIndexed(indexbuffer->size_);
}

void Drawable::AddBind(std::unique_ptr<BindableInterface> bind)
{
	binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuf(std::unique_ptr<IndexBuffer> ibf, Graphics& gfx)
{
	indexbuffer = ibf.get();
	indexbuffer->Bind(gfx);
}

Drawable::~Drawable()
{

}

WorldTransform& Drawable::GetTransform()
{
	return transform;
}

void Drawable::SetWorldLocation(const CusMath::vector3d& t)
{
	world_location_ = t;
}

void Drawable::AddWorldLocation(const CusMath::vector3d& t)
{
	world_location_ += t;
}

void Drawable::SetActorLocation(const CusMath::vector3d& t)
{
	object_location_ = t;
}

void Drawable::SetWorldRotation(const CusMath::vector3d& r)
{
	world_rotation_ = r;
}

void Drawable::AddWorldRotation(const CusMath::vector3d& r)
{
	world_rotation_ += r;
}

void Drawable::AddActorRotation(const CusMath::vector3d& r)
{
	object_rotation_ += r;
}

void Drawable::SetActorRotation(const CusMath::vector3d& r)
{
	object_rotation_ = r;
}

void Drawable::SetActorScale(const CusMath::vector3d& s)
{
	scale_ = s;
}

void Drawable::AddActorLocation(const CusMath::vector3d& t)
{
	object_location_ += t;
}

const CusMath::vector3d Drawable::GetActorLocation() const
{
	return object_location_;
}

const CusMath::vector3d Drawable::GetWorldLocation() const
{
	return world_location_;
}

const CusMath::vector3d Drawable::GetActorRotation() const
{
	return object_rotation_;
}

const CusMath::vector3d Drawable::GetWorldRotation() const
{
	return world_rotation_;
}

const CusMath::vector3d Drawable::GetWorldScale() const
{
	return scale_;
}

DirectX::XMMATRIX Drawable::GetTranslateMartix() const
{
	return DonedTransforms[0];
}

DirectX::XMMATRIX Drawable::GetRotationMartix() const
{
	return 	DirectX::XMMATRIX(right_direction_.x, right_direction_.y, right_direction_.z, 0.f,
		up_direction_.x, up_direction_.y, up_direction_.z, 0.f,
		forward_direction_.x, forward_direction_.y, forward_direction_.z, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

DirectX::XMFLOAT3 Drawable::forward_direction() const
{
	return forward_direction_;
}

DirectX::XMFLOAT3 Drawable::right_direction() const
{
	return right_direction_;
}

DirectX::XMFLOAT3 Drawable::up_direction() const
{
	return up_direction_;
}

bool Drawable::IsOnWorldCoordinate() const
{
	return !(forward_direction_.x != 0.f || forward_direction_.y != 0.f || forward_direction_.z != 1.f ||
		right_direction_.x != 1.f || right_direction_.y != 0.f || right_direction_.z != 0.f ||
		up_direction_.x != 0.f || up_direction_.y != 1.f || up_direction_.z != 0.f);
}

