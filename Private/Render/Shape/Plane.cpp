#include "Public/Render/Bindable/VertexBuffer.hpp"
#include "Public/Render/Bindable/VertexShader.h"
#include "Public/Render/Bindable/PixelShader.h"
#include "Public/Render/Bindable/ConstantBuffer.h"
#include "Public/Render/VertexLayout.h"
#include "Public/Render/Bindable/InputLayout.h"
#include "Public/Render/Bindable/TransformBuffer.h"
#include "Public/Global.h"
#include "Public\Render\Shape\Plane.h"
#include "Public/Render/Shape/Shape.hpp"
#include "Public/Render/Graphics.h"
#include "Public/Render/GraphicsResource.h"

template<typename T>
using Vec = std::vector<T, std::allocator<T>>;
using BindItem = std::unique_ptr<BindableInterface>;

Plane::Plane(int row, int col, const int& size, Graphics& gfx)
{
	if (!isInitialzed())
	{
		std::vector<Postion3DColored> vertices;
		Postion3DColored vertex;
		for (int i = 0; i <= row; i++)
		{
			for (int j = 0; j <= col; j++)
			{
				vertex.pos = { (float)(-size / 2 + j * (size / col)) ,0.f,float(size / 2 - i * (size / row)) };
				vertex.color = { 1.f,1.f,1.f };
				vertices.push_back(vertex);
			}
		}
		BindItem vb =
			std::make_unique<VertexBuffer<Postion3DColored, Vec>>(vertices, gfx);
		AddStaticBind(std::move(vb));
		std::vector<UINT> indices;
		for (int i = 0; i < vertices.size(); i++)
		{
			if (i % (col+1) < col && i / (row+1) < row)
			{
				indices.push_back(i);
				indices.push_back(i+1);
				indices.push_back(i+col+1);
				indices.push_back(i+1);
				indices.push_back(i + col + 2);
				indices.push_back(i +col+ 1);
			}
		}
		auto ib = std::make_unique<IndexBuffer>(indices, gfx);
		AddStaticIndexBuf(std::move(ib), gfx);

		BindItem vs = std::make_unique<VertexShader>(gfx, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/VertexShader.cso");
		BindItem ps = std::make_unique<PixelShader>(gfx, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/PixelShader.cso");
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
	world_location_ = {0.f,0.f,0.f};
	world_rotation_ = { 0.f,0.f,0.f };
	scale_ = { 1.f,1.f,1.f };
	transform =
	{
		view *
		projection
	};
	BindItem vcb = std::make_unique<TransformBuffer>(gfx, *this);
	AddBind(std::move(vcb));
}
