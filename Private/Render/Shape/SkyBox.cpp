#include "Public/Render/Shape/SkyBox.h"
#include <Public/Render/Bindable/BindableInterface.h>
#include <Public/Render/VertexLayout.h>
#include <Public/Render/Bindable/VertexBuffer.hpp>
#include <Public/Render/Bindable/IndexBuffer.h>
#include <Public/Render/Bindable/VertexShader.h>
#include <Public/Render/Bindable/PixelShader.h>
#include <Public/Render/Bindable/InputLayout.h>
#include <Public/Render/Bindable/TransformBuffer.h>
#include <Public/Tool/ResLoader.h>
#include <Public/Render/Bindable/ShaderResource.h>
#include <Public/Render/Bindable/RasterizerState.h>
#include <Public/Render/Bindable/DepthStencilState.h>




template<typename T>
using Vec = std::vector<T, std::allocator<T>>;
using BindItem = std::unique_ptr<BindableInterface>;

namespace
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
}


SkyBox::SkyBox(Graphics& gfx)
{
	cast_shadow_ = false;
	const CusMath::vector3d initPos(0.f, 0.f, 0.f);
	float size = 100.f;
	//if (!isInitialzed())
	//{
		std::vector<Postion3D> vertices
		{
			{{initPos.x + size,initPos.y + size,initPos.z + size}},
			{{initPos.x + size,initPos.y + size,initPos.z - size}},
			{{initPos.x - size,initPos.y + size,initPos.z - size}},
			{{initPos.x - size,initPos.y + size,initPos.z + size}},
			{{initPos.x + size,initPos.y - size,initPos.z + size}},
			{{initPos.x + size,initPos.y - size,initPos.z - size}},
			{{initPos.x - size,initPos.y - size,initPos.z - size}},
			{{initPos.x - size,initPos.y - size,initPos.z + size}}
		};
		BindItem vb =
			std::make_unique<VertexBuffer<Postion3D, Vec>>(vertices, gfx);
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
			//3,1,0,
			//3,2,1,//top
			//1,4,0,
			//1,5,4,//right
			//2,5,1,
			//2,6,5,//front
			//6,4,5,
			//6,7,4,//bot
			//3,6,2,
			//3,7,6,//left
			//0,7,3,
			//7,0,4//back
		};
		auto ib = std::make_unique<IndexBuffer>(indices, gfx);
		AddStaticIndexBuf(std::move(ib), gfx);

		BindItem vs = std::make_unique<VertexShader>(gfx, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/vSkyBox.cso");
		BindItem ps = std::make_unique<PixelShader>(gfx, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/pSkyBox.cso");
		AddStaticBind(std::move(ps));

		VertexLayout vl;
		vl << EVertexType::kPosition3D;
		BindItem il = std::make_unique<InputLayout>(gfx, *dynamic_cast<VertexShader*>(vs.get()), vl, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		AddStaticBind(std::move(vs));
		AddStaticBind(std::move(il));
		//test
		view = gfx.p_camera_->view_matrix();
		projection = gfx.p_camera_->projection_matrix();
	
	//else
	//{
	//	SetIndexbufferFromSBinds();
	//}
	world_location_ = initPos;
	world_rotation_ = { 0.f,0.f,0.f };
	scale_ = { 1.f,1.f,1.f };
	v_cons_buf_.mvp_matrix_ =
	{
		DirectX::XMMatrixTranslation(initPos.x,initPos.y,initPos.z) *
		view *
		projection
	};
	BindItem vcb = std::make_unique<TransformBuffer>(gfx, *this);
	AddBind(std::move(vcb));
	//SetWorldLocation(initPos);

	//create single texture for 6 face
	D3D11_TEXTURE2D_DESC des{};
	des.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	des.Width = 2048;
	des.Height = 2048;
	des.ArraySize = 1;
	des.MipLevels = 1;
	des.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	des.Usage = D3D11_USAGE_DEFAULT;
	des.CPUAccessFlags = 0;
	des.SampleDesc.Count = 1;
	des.SampleDesc.Quality = 0;
	tex_top = std::move(std::make_unique<Texture>(gfx.GetDevice(), "Y:/Project_VS2019/DX11RenderEngine/Res/Texture/top.jpg", des));
	tex_bottom = std::move(std::make_unique<Texture>(gfx.GetDevice(), "Y:/Project_VS2019/DX11RenderEngine/Res/Texture/bottom.jpg", des));
	tex_right = std::move(std::make_unique<Texture>(gfx.GetDevice(), "Y:/Project_VS2019/DX11RenderEngine/Res/Texture/right.jpg", des));
	tex_left = std::move(std::make_unique<Texture>(gfx.GetDevice(), "Y:/Project_VS2019/DX11RenderEngine/Res/Texture/left.jpg", des));
	tex_front = std::move(std::make_unique<Texture>(gfx.GetDevice(), "Y:/Project_VS2019/DX11RenderEngine/Res/Texture/front.jpg", des));
	tex_back = std::move(std::make_unique<Texture>(gfx.GetDevice(), "Y:/Project_VS2019/DX11RenderEngine/Res/Texture/back.jpg", des));
	tex_vec_.push_back(tex_right.get());
	tex_vec_.push_back(tex_left.get());
	tex_vec_.push_back(tex_top.get());
	tex_vec_.push_back(tex_bottom.get());
	tex_vec_.push_back(tex_front.get());
	tex_vec_.push_back(tex_back.get());
	//create cube desc and texture2d
	D3D11_TEXTURE2D_DESC cube_des{};
	cube_des.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	cube_des.Width = 2048;
	cube_des.Height = 2048;
	cube_des.ArraySize = 6;	//*
	cube_des.MipLevels = 1;
	cube_des.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	cube_des.Usage = D3D11_USAGE_DEFAULT;
	cube_des.CPUAccessFlags = 0;
	cube_des.SampleDesc.Count = 1;
	cube_des.SampleDesc.Quality = 0;
	cube_des.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;	//*
	Microsoft::WRL::ComPtr<ID3D11Texture2D> cube_texture;
	gfx.GetDevice()->CreateTexture2D(&cube_des, nullptr, cube_texture.GetAddressOf());
	//copy resource form file to current
	for (int i = 0; i < 6; ++i)
	{
		gfx.GetContext()->CopySubresourceRegion(cube_texture.Get(), D3D11CalcSubresource(0u,i, 1u)
			, 0, 0, 0, tex_vec_[i]->GetResource(), 0, nullptr);
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC rsv;
	rsv.Format = des.Format;
	rsv.TextureCube.MipLevels = 1;
	rsv.TextureCube.MostDetailedMip = 0;
	rsv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;	//*
	gfx.GetDevice()->CreateShaderResourceView(cube_texture.Get(), &rsv, srv.GetAddressOf());
	AddBind(std::make_unique<ShaderResource>(srv.Get(), ETextureType::kDiffuse));
	D3D11_RASTERIZER_DESC rds{};
	rds.FillMode = D3D11_FILL_SOLID;
	rds.CullMode = D3D11_CULL_NONE;
	AddBind(std::make_unique<RasterizerState>(gfx, rds));
	D3D11_DEPTH_STENCIL_DESC dsds{};
	dsds.DepthEnable = true;
	//enable depth read and test
	dsds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	AddBind(std::make_unique<DepthStencilState>(gfx, dsds));
}
