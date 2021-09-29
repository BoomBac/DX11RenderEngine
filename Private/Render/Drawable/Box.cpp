#include "Public/Render/Bindable/VertexBuffer.hpp"
#include "Public/Render/Bindable/VertexShader.h"
#include "Public/Render/Bindable/PixelShader.h"
#include "Public/Render/Bindable/ConstantBuffer.h"
#include "Public/Render/VertexLayout.h"
#include "Public/Render/Bindable/InputLayout.h"
#include "Public/Render/Bindable/TransformBuffer.h"
#include "Public/Global.h"
#include "Public/Render/Shape/Box.h"
#include "Public/Render/Shape/Shape.hpp"
#include "Public/Render/Graphics.h"
#include "Public/Render/GraphicsResource.h"



template<typename T>
using Vec = std::vector<T, std::allocator<T>>;
using BindItem = std::unique_ptr<BindableInterface>;


Box::Box(const int& size,Graphics& gfx)
{
	const CusMath::vector3d initPos(0.f, 0.f, 0.f);
	if (!isInitialzed())
	{
		std::vector<Postion3DColored> vertices
		{
			{{initPos.x + size,initPos.y + size,initPos.z + size},{1.f,0.f,0.f}},
			{{initPos.x + size,initPos.y + size,initPos.z - size},{1.f,0.f,0.f}},
			{{initPos.x - size,initPos.y + size,initPos.z - size},{1.f,0.f,0.f}},
			{{initPos.x - size,initPos.y + size,initPos.z + size},{1.f,0.f,0.f}},
			{{initPos.x + size,initPos.y - size,initPos.z + size},{0.f,1.f,0.f}},
			{{initPos.x + size,initPos.y - size,initPos.z - size},{0.f,1.f,0.f}},
			{{initPos.x - size,initPos.y - size,initPos.z - size},{0.f,1.f,0.f}},
			{{initPos.x - size,initPos.y - size,initPos.z + size},{0.f,1.f,0.f}}
		};
		BindItem vb =
			std::make_unique<VertexBuffer<Postion3DColored, Vec>>(vertices, gfx);
		AddStaticBind(std::move(vb));
		std::vector<UINT> indices
		{
			3,0,1,
			3,1,2,//top
			1,0,4,
			1,4,5,//right
			2,1,5,
			2,5,6,//front
			6,5,4,
			6,4,7,//bot
			3,2,6,
			3,6,7,//left
			0,3,7,
			7,4,0//back
		};
		auto ib = std::make_unique<IndexBuffer>(indices, gfx);
		AddStaticIndexBuf(std::move(ib), gfx);

		BindItem vs = std::make_unique<VertexShader>(gfx, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/InsideVshader.cso");
		BindItem ps = std::make_unique<PixelShader>(gfx, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/InsidePshader.cso");
		AddStaticBind(std::move(ps));

		VertexLayout vl;
		vl << VertexType::Position3D << VertexType::Float3Color;
		BindItem il = std::make_unique<InputLayout>(gfx, *dynamic_cast<VertexShader*>(vs.get()), vl, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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
	world_location_ = initPos;
	world_rotation_ = { 0.f,0.f,0.f };
	scale_ = { 1.f,1.f,1.f };
	transform =
	{
		DirectX::XMMatrixTranslation(initPos.x,initPos.y,initPos.z)*
		view*
		projection
	};
	BindItem vcb = std::make_unique<TransformBuffer>(gfx,*this);
	AddBind(std::move(vcb));
	SetWorldLocation(initPos);
}


