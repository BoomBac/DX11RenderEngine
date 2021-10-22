#include "Public/Render/Drawable/Drawable.h"
#include "Public/Tool/Utili.h"
#include "Public/Render/Graphics.h"
#include "Public/Render/Bindable/BindableInterface.h"
#include "Public/Render/Bindable/IndexBuffer.h"
#include "Public/Render/ResManage/TextureFactory.h"
#include "Public/Render/Bindable/ConstantBuffer.h"


namespace dx = DirectX;

DirectX::XMMATRIX Drawable::view;
DirectX::XMMATRIX Drawable::projection;

void Drawable::Draw(Graphics& gfx)
{
	if (!visiblity_) return;
	//根据pass类型选择camera
	if (gfx.isRenderShaodw)
	{
		if (cast_shadow_)
		{
			view = gfx.p_light_camera->view_matrix();
			projection = gfx.p_light_camera->projection_matrix();
			v_cons_buf_.camera_pos = gfx.p_light_camera->location_f();
		}
	}
	else
	{
		view = gfx.p_camera_->view_matrix();// *gfx.p_camera_->projection_matrix();
		projection = gfx.p_camera_->projection_matrix();
		v_cons_buf_.camera_pos = gfx.p_camera_->location_f();
	}

	int pc_id = -1;
	int vc_id = -1;
	Update();
	//记录可绑定多个bind的数量，并将他们绑定到不同的slot
	for (auto& i : binds)
	{
		if (i->GetType() == EBindableType::kPixelConstantBuffer)
		{
			++pc_id;
			i->pc_buf_index = &pc_id;
		}
		if (i->GetType() == EBindableType::kVetexConstantBuffer)
		{
			++vc_id;
			i->vc_buf_index_ = &vc_id;
		}
		i->Bind(gfx);
	}
	for (auto& i : GetStaticBinds())
	{
		if (i->GetType() == EBindableType::kPixelConstantBuffer)
		{
			++pc_id;
			i->pc_buf_index = &pc_id;
		}
		if (i->GetType() == EBindableType::kVetexConstantBuffer)
		{
			++vc_id;
			i->vc_buf_index_ = &vc_id;
		}
		i->Bind(gfx);
	}
	//生成shadowMap时PixelShader置空，正常渲染时将shadowMap绑定到着色器资源
	if (gfx.isRenderShaodw)
	{
		gfx.GetContext()->PSSetShader(nullptr, nullptr, 0u);
	}
	else
	{
		//gfx.GetContext()->PSSetShaderResources(0, 1, gfx.GetShadowMap());
	}

	if (gfx.isRenderShaodw)
	{
		if (cast_shadow_)
		{
			gfx.DrawIndexed(indexbuffer->size_);
		}
	}
	else
		gfx.DrawIndexed(indexbuffer->size_);
	gfx.GetContext()->RSSetState(nullptr);
	gfx.GetContext()->OMSetDepthStencilState(nullptr, 1u);
	//渲染结束将shadowMap槽位的shaderResource解绑，以便其在下一个shadowPass可以用作depthbuffer
	//gfx.GetContext()->PSSetShaderResources(0, 1, TextureFactory::GetInstance().GetTexture("Depth.png")->GetTextureResourceViewAddress());
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
	return v_cons_buf_;
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

