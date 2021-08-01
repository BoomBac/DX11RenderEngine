#include "Box.h"
#include "VertexBuffer.hpp"
#include "VertexShader.h"
#include "PixelShader.h"
#include <DirectXMath.h>
#include "ConstantBuffer.h"
#include "VertexLayout.h"
#include "InputLayout.h"

template<typename T>
using Vec = std::vector<T, std::allocator<T>>;
using BindItem = std::unique_ptr<Bindable>;


struct ConstantBuffers
{
	DirectX::XMMATRIX mWorld; //4x4
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
};

Box::Box(const CusMath::vector3d& initPos, const int& size,Graphics& gfx)
{
	std::vector<CusMath::vector3d> vertices
	{
		{initPos.x + size,initPos.y + size,initPos.z + size},
		{initPos.x + size,initPos.y + size,initPos.z - size},
		{initPos.x - size,initPos.y + size,initPos.z - size},
		{initPos.x - size,initPos.y + size,initPos.z + size},
		{initPos.x + size,initPos.y - size,initPos.z + size},
		{initPos.x + size,initPos.y - size,initPos.z - size},
		{initPos.x - size,initPos.y - size,initPos.z - size},
		{initPos.x - size,initPos.y - size,initPos.z + size},
	};
	BindItem vb =
		std::make_unique<VertexBuffer<CusMath::vector3d, Vec>>(vertices, gfx);
	AddBind(std::move(vb));
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
	BindItem ib = std::make_unique<IndexBuffer>(indices, gfx);
	AddBind(std::move(ib));

	BindItem vs = std::make_unique<VertexShader>(gfx, "VertexShader.cso");
	AddBind(std::move(vs));

	BindItem ps = std::make_unique<PixelShader>(gfx, "PixelShader.cso");
	AddBind(std::move(ps));
	DirectX::XMVECTOR Eye = DirectX::XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
	DirectX::XMVECTOR At = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	ConstantBuffers cb
	{
		DirectX::XMMatrixIdentity(),
		DirectX::XMMatrixLookAtLH(Eye, At, Up),
		DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 4.0f / 3.0f, 0.01f, 100.f)
	};
	BindItem vcb = std::make_unique<VConstantBuffer<ConstantBuffers>>(gfx, cb);
	AddBind(std::move(vcb));

	VertexLayout vl;
	vl << VertexType::Position3D;
	vl.Build();
	BindItem il = std::make_unique<InputLayout>(gfx,*dynamic_cast<VertexShader*>(vs.get()),vl);
	AddBind(std::move(il));
}
