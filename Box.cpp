#include "Box.h"
#include "VertexBuffer.hpp"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "VertexLayout.h"
#include "InputLayout.h"
#include "TransformBuffer.h"
#include "Global.h"


template<typename T>
using Vec = std::vector<T, std::allocator<T>>;
using BindItem = std::unique_ptr<Bindable>;


Box::Box(const CusMath::vector3d& initPos, const int& size,Graphics& gfx)
{
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

		BindItem vs = std::make_unique<VertexShader>(gfx, "VertexShader.cso");
		BindItem ps = std::make_unique<PixelShader>(gfx, "PixelShader.cso");
		AddStaticBind(std::move(ps));

		VertexLayout vl;
		vl << VertexType::Position3D << VertexType::Float3Color;
		BindItem il = std::make_unique<InputLayout>(gfx, *dynamic_cast<VertexShader*>(vs.get()), vl);
		AddStaticBind(std::move(vs));
		AddStaticBind(std::move(il));
	}
	else
	{
		SetIndexbufferFromSBinds();
	}
	Location = { 0.f,0.f,0.f };
	Rotation = { 0.f,0.f,0.f };
	Scale = { 1.f,1.f,1.f };
	//DirectX::XMVECTOR Eye = DirectX::XMVectorSet(0.0f,50.0f, 0.0f, 0.0f);
	//DirectX::XMVECTOR At = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	transform = 
	{
		DirectX::XMMatrixIdentity()*
		view*
		projection
	};
	BindItem vcb = std::make_unique<TransformBuffer>(gfx,*this);
	AddBind(std::move(vcb));

}


