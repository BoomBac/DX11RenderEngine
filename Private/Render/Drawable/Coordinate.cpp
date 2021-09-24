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
	if (!isInitialzed())
	{
		CusMath::vector3d pos = gfx.p_selected_object_ == nullptr ? CusMath::vector3d{0.f, 0.f, 0.f} : gfx.p_selected_object_->GetWorldLocation();
		object_attached_ = gfx.p_selected_object_;
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

		BindItem vs = std::make_unique<VertexShader>(gfx, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/VertexShader.cso");
		BindItem ps = std::make_unique<PixelShader>(gfx, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/PixelShader.cso");
		AddStaticBind(std::move(ps));

		VertexLayout vl;
		vl << VertexType::Position3D << VertexType::Float3Color;
		BindItem il = std::make_unique<InputLayout>(gfx, *dynamic_cast<VertexShader*>(vs.get()), vl, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		AddStaticBind(std::move(vs));
		AddStaticBind(std::move(il));
		//test
		view = gfx.camera_.view_matrix();
		projection = gfx.camera_.projection_matrix();
	}
	else
	{
		SetIndexbufferFromSBinds();
	}
	world_location_ = { 0.f,0.f,0.f };
	world_rotation_ = { 0.f,0.f,0.f };
	Scale = { 1.f,1.f,1.f };
	transform =
	{
		DirectX::XMMatrixIdentity() *
		view *
		projection
	};
	BindItem vcb = std::make_unique<TransformBuffer>(gfx, *this);
	AddBind(std::move(vcb));
	is_world_ = true;
}

void Coordinate::Draw(Graphics& gfx)
{
	transform.mWorld = object_attached_->GetTranslateMartix() * transform.mWorld;
	if (!is_world_)
		transform.mWorld = object_attached_->GetRotationMartix() * transform.mWorld;

	Drawable::Draw(gfx);
}

void Coordinate::SetCoordinateType(bool is_world)
{
	is_world_ = is_world;
}

