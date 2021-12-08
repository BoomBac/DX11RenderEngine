#include <Public/Render/Drawable/Coordinate.h>
#include <Public/Render/VertexLayout.h>
#include <Public/Render/Bindable/VertexBuffer.hpp>
#include <Public/Render/Bindable/VertexShader.h>
#include <Public/Render/Bindable/PixelShader.h>
#include <Public/Render/Bindable/TransformBuffer.h>
#include "Public/Render/Bindable/InputLayout.h"

template<typename T>
using Vec = std::vector<T, std::allocator<T>>;
using BindItem = std::unique_ptr<BindableInterface>;

Coordinate::Coordinate(Graphics& gfx, const float& size)
{
	cast_shadow_ = false;
	if (!isInitialzed())
	{
		CusMath::vector3d pos = gfx.p_selected_object_ == nullptr ? CusMath::vector3d{0.f, 0.f, 0.f} : gfx.p_selected_object_->GetWorldLocation();
		p_object_attached_ = gfx.p_selected_object_;
		std::vector<Postion3DColored> vertices
		{
			{pos,{0.f,0.f,0.f} },
			{{pos.x,pos.y + 0.1f,pos.z},{0.f,1.f,0.f} },
			{{pos.x + 0.1f,pos.y,pos.z},{1.f,0.f,0.f} },
			{{pos.x,pos.y,pos.z + 0.1f},{0.f,0.f,1.f} },
			{{pos.x,pos.y + size,pos.z},{0.f,1.f,0.f} },
			{{pos.x + size,pos.y,pos.z},{1.f,0.f,0.f} },
			{{pos.x,pos.y,pos.z + size},{0.f,0.f,1.f} },
			//TODO() : 默认出现世界坐标系，此处应该根据物体大小决定轴长度
			//{{0.f,10.f,0.f},{0.f,1.f,0.f} },
			//{{10.f,0.f,0.f},{1.f,0.f,0.f} },
			//{{0.f,0.f,10.f},{0.f,0.f,1.f} }

		};
		BindItem vb =
			std::make_unique<VertexBuffer<Postion3DColored, Vec>>(vertices, gfx);
		AddStaticBind(std::move(vb));
		std::vector<UINT> indices
		{
			//两点连一个线段
			0,1,1,4,
			0,3,3,6,
			0,2,2,5
		};
		auto ib = std::make_unique<IndexBuffer>(indices, gfx);
		AddStaticIndexBuf(std::move(ib), gfx);

		BindItem vs = std::make_unique<VertexShader>(gfx, "E:/Project_VS2019/DX11RenderEngine/Shaders/cso/RawColor.cso");
		BindItem ps = std::make_unique<PixelShader>(gfx, "E:/Project_VS2019/DX11RenderEngine/Shaders/cso/InsidePshader.cso");
		AddStaticBind(std::move(ps));

		VertexLayout vl;
		vl << EVertexType::kPosition3D << EVertexType::kFloat3Color;
		BindItem il = std::make_unique<InputLayout>(gfx, *dynamic_cast<VertexShader*>(vs.get()), vl, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		AddStaticBind(std::move(vs));
		AddStaticBind(std::move(il));
		//test
		view = gfx.p_camera_->view_matrix();
		projection = gfx.p_camera_->projection_matrix();
	}
	else
	{
		return;
		SetIndexbufferFromSBinds();
	}
	world_location_ = { 0.f,0.f,0.f };
	world_rotation_ = { 0.f,0.f,0.f };
	scale_ = { 1.f,1.f,1.f };
	v_cons_buf_.world_matrix_ = DirectX::XMMatrixIdentity();
	BindItem vcb = std::make_unique<TransformBuffer>(gfx, *this);
	vcb->vc_buf_index_ = 1u;
	AddBind(std::move(vcb));
	is_world_ = true;
	effects_.push_back(EEffectType::kNone);
}

void Coordinate::Draw(Graphics& gfx)
{
	if (p_object_attached_ == nullptr) return;
	DonedTransforms[0] = p_object_attached_->GetTranslateMartix();
	if (!is_world_)
		DonedTransforms[1] = p_object_attached_->GetRotationMartix();
	Drawable::Draw(gfx);
}

void Coordinate::SetCoordinateType(bool is_world)
{
	is_world_ = is_world;
}

bool Coordinate::GetCoordinateType() const
{
	return is_world_;
}

void Coordinate::SetAttachedObject(Drawable* object)
{
	p_object_attached_ = object;
}

