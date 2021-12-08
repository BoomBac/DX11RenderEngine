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
#include "Public/Render/Bindable/ConstantBuffer.h"
#include "Public/Render/Bindable/LightBuffer.h"
#include "Public/Render/ResManage/TextureFactory.h"

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
				vertex.color = { (vertex.pos.x + (float)(size / 2)) / (float)size,abs((vertex.pos.z - (float)(size / 2)) / (float)size),0.f };
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

		BindItem vs = std::make_unique<VertexShader>(gfx, "E:/Project_VS2019/DX11RenderEngine/Shaders/cso/InsideVshader.cso");
		BindItem ps = std::make_unique<PixelShader>(gfx, "E:/Project_VS2019/DX11RenderEngine/Shaders/cso/Lambert.cso");
		AddStaticBind(std::move(ps));

		VertexLayout vl;
		vl << EVertexType::kPosition3D << EVertexType::kFloat3Color;
		BindItem il = std::make_unique<InputLayout>(gfx, *dynamic_cast<VertexShader*>(vs.get()), vl, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		AddStaticBind(std::move(vs));
		AddStaticBind(std::move(il));
		BindItem pcb = std::make_unique<PSConstantBuffer<LightSet>>(gfx,&gfx.p_scene_light_);
		AddStaticBind(std::move(pcb));
		//test
		view = gfx.p_camera_->view_matrix();
		projection = gfx.p_camera_->projection_matrix();

		//textures_.push_back(TextureFactory::GetInstance().GetTexture("height.jpg"));
		//gfx.GetContext()->PSSetShaderResources(0, 1, textures_[0]->GetTextureResourceViewAddress());
		

	}
	else
	{
		SetIndexbufferFromSBinds();
	}
	world_location_ = {0.f,0.f,0.f};
	world_rotation_ = { 0.f,0.f,0.f };
	scale_ = { 1.f,1.f,1.f };
	v_cons_buf_.world_matrix_ = DirectX::XMMatrixIdentity();
	BindItem vcb = std::make_unique<TransformBuffer>(gfx, *this);
	AddBind(std::move(vcb));
}
void Plane::Draw(Graphics& gfx)
{
	//gfx.GetContext()->PSSetShaderResources(0, 1, gfx.GetShadowMap());
	//gfx.GetContext()->PSSetShaderResources(0, 1, textures_[0]->GetTextureResourceViewAddress());
	Drawable::Draw(gfx);
}