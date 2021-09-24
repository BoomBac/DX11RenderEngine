#include "Public/Render/Drawable/Drawable.h"
#include "Public/Tool/Utili.h"
#include "Public/Render/Graphics.h"
#include "Public/Render/Bindable/BindableInterface.h"
#include "Public/Render/Bindable/IndexBuffer.h"


namespace dx = DirectX;
//eyePos,lookAt,upDirection   test
DirectX::XMMATRIX Drawable::view;
DirectX::XMMATRIX Drawable::projection;

static dx::XMVECTOR g_camera_location = { 0.f,0.f,50.f };

void Drawable::Draw(Graphics& gfx)
{
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
	Scale = s;
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

DirectX::XMMATRIX Drawable::GetTranslateMartix() const
{
	return DonedTransforms[0];
}

DirectX::XMMATRIX Drawable::GetRotationMartix() const
{
	return 	DirectX::XMMATRIX(right_direction.x, right_direction.y, right_direction.z, 0.f,
		up_direction.x, up_direction.y, up_direction.z, 0.f,
		forward_direction.x, forward_direction.y, forward_direction.z, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

