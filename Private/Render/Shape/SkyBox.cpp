
#define STB_IMAGE_IMPLEMENTATION
#include "External/stb_image.h"

#include "Public/Render/Shape/SkyBox.h"
#include <Public/Render/Bindable/BindableInterface.h>
#include <Public/Render/VertexLayout.h>
#include <Public/Render/Bindable/VertexBuffer.hpp>
#include <Public/Render/Bindable/IndexBuffer.h>
#include <Public/Render/Bindable/VertexShader.h>
#include <Public/Render/Bindable/PixelShader.h>
#include <Public/Render/Bindable/InputLayout.h>
#include <Public/Render/Bindable/TransformBuffer.h>
#include <Public/Render/Bindable/ShaderResource.h>
#include <Public/Render/Bindable/RasterizerState.h>
#include <Public/Render/Bindable/DepthStencilState.h>
#include <Public/Render/RenderToTexture.h>





template<typename T>
using Vec = std::vector<T, std::allocator<T>>;
using BindItem = std::unique_ptr<BindableInterface>;

namespace
{
	static Graphics* g_gfx;
}


SkyBox::SkyBox(Graphics& gfx)
{
	cast_shadow_ = false;
	const CusMath::vector3d initPos(0.f, 0.f, 0.f);
	float size = 1000.f;
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
	BindItem vb = std::make_unique<VertexBuffer<Postion3D, Vec>>(vertices, gfx);
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
	//SetActorScale({ 100.f,100.f,100.f });
	g_gfx = &gfx;
	D3D11_RASTERIZER_DESC rds{};
	rds.FillMode = D3D11_FILL_SOLID;
	rds.CullMode = D3D11_CULL_NONE;
	AddBind(std::make_unique<RasterizerState>(*g_gfx, rds));
	D3D11_DEPTH_STENCIL_DESC dsds{};
	dsds.DepthEnable = true;
	//enable depth read and test
	dsds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	AddBind(std::make_unique<DepthStencilState>(*g_gfx, dsds));
	image_paths_.push_back("C:/Users/BoomBac/Desktop/top.png");
	image_paths_.push_back("C:/Users/BoomBac/Desktop/bottom.png");
	image_paths_.push_back("C:/Users/BoomBac/Desktop/right.png");
	image_paths_.push_back("C:/Users/BoomBac/Desktop/left.png");
	image_paths_.push_back("C:/Users/BoomBac/Desktop/front.png");
	image_paths_.push_back("C:/Users/BoomBac/Desktop/back.png");
	TexturedFromCube();
}

ID3D11ShaderResourceView* SkyBox::GetEnvironmentRsv() const
{
	return p_srv_.Get();
}

void SkyBox::GenerateCubeMap()
{
	Camera* cur_camera = g_gfx->p_camera_;
	Camera sample_camera;
	sample_camera.SetLocation(0.f, 0.f, 0.f);
	sample_camera.SetProjection(90.f, 1.f, 1.f, 1000.f);
	std::unique_ptr<RenderToTexture> rtt = std::make_unique<RenderToTexture>();
	rtt->Initialize(g_gfx, ERTTUsage::kBackBuffer);
	rtt->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	rtt->SetRenderTarget(g_gfx);
	g_gfx->isRenderShaodw = false;
	g_gfx->p_camera_ = &sample_camera;
	Draw(*g_gfx);
	rtt->SaveToImage(g_gfx, "C:/Users/BoomBac/Desktop/front.png");
	sample_camera.SetRotation(DegToRad(0.f), DegToRad(180.f), DegToRad(0.f));
	Draw(*g_gfx);
	rtt->SaveToImage(g_gfx, "C:/Users/BoomBac/Desktop/back.png");
	sample_camera.SetRotation(DegToRad(0.f), DegToRad(-90.f), DegToRad(0.f));
	Draw(*g_gfx);
	rtt->SaveToImage(g_gfx, "C:/Users/BoomBac/Desktop/left.png");
	sample_camera.SetRotation(DegToRad(0.f), DegToRad(90.f), DegToRad(0.f));
	Draw(*g_gfx);
	rtt->SaveToImage(g_gfx, "C:/Users/BoomBac/Desktop/right.png");
	sample_camera.SetRotation(DegToRad(-90.f), DegToRad(0.f), DegToRad(0.f));
	Draw(*g_gfx);
	rtt->SaveToImage(g_gfx, "C:/Users/BoomBac/Desktop/top.png");
	sample_camera.SetRotation(DegToRad(90.f), DegToRad(0.f), DegToRad(0.f));
	Draw(*g_gfx);
	rtt->SaveToImage(g_gfx, "C:/Users/BoomBac/Desktop/bottom.png");
	g_gfx->isRenderShaodw = true;
	g_gfx->p_camera_ = cur_camera;
}

void SkyBox::TexturedFromCube()
{
	int w = 600;
	int h = 600;
	//create single texture for 6 face
	D3D11_TEXTURE2D_DESC des{};
	des.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	des.Width = w;
	des.Height = h;
	des.ArraySize = 1;
	des.MipLevels = 1;
	des.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	des.Usage = D3D11_USAGE_DEFAULT;
	des.CPUAccessFlags = 0;
	des.SampleDesc.Count = 1;
	des.SampleDesc.Quality = 0;
	//tex_top = std::move(std::make_unique<Texture>(g_gfx->GetDevice(), image_paths_[0].c_str(), des));
	//tex_bottom = std::move(std::make_unique<Texture>(g_gfx->GetDevice(), "Y:/Project_VS2019/DX11RenderEngine/Res/Texture/bottom.jpg", des));
	//tex_right = std::move(std::make_unique<Texture>(g_gfx->GetDevice(), "Y:/Project_VS2019/DX11RenderEngine/Res/Texture/right.jpg", des));
	//tex_left = std::move(std::make_unique<Texture>(g_gfx->GetDevice(), "Y:/Project_VS2019/DX11RenderEngine/Res/Texture/left.jpg", des));
	//tex_front = std::move(std::make_unique<Texture>(g_gfx->GetDevice(), "Y:/Project_VS2019/DX11RenderEngine/Res/Texture/front.jpg", des));
	//tex_back = std::move(std::make_unique<Texture>(g_gfx->GetDevice(), "Y:/Project_VS2019/DX11RenderEngine/Res/Texture/back.jpg", des));
	tex_top = std::move(std::make_unique<Texture>(g_gfx->GetDevice(), image_paths_[0].c_str(), des));
	tex_bottom = std::move(std::make_unique<Texture>(g_gfx->GetDevice(), image_paths_[1].c_str(), des));
	tex_right = std::move(std::make_unique<Texture>(g_gfx->GetDevice(), image_paths_[2].c_str(), des));
	tex_left = std::move(std::make_unique<Texture>(g_gfx->GetDevice(), image_paths_[3].c_str(), des));
	tex_front = std::move(std::make_unique<Texture>(g_gfx->GetDevice(), image_paths_[4].c_str(), des));
	tex_back = std::move(std::make_unique<Texture>(g_gfx->GetDevice(), image_paths_[5].c_str(), des));
	tex_vec_.push_back(tex_right.get());
	tex_vec_.push_back(tex_left.get());
	tex_vec_.push_back(tex_top.get());
	tex_vec_.push_back(tex_bottom.get());
	tex_vec_.push_back(tex_front.get());
	tex_vec_.push_back(tex_back.get());
	//create cube desc and texture2d
	D3D11_TEXTURE2D_DESC cube_des{};
	cube_des.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	cube_des.Width = w;
	cube_des.Height = h;
	cube_des.ArraySize = 6;	//*
	cube_des.MipLevels = 1;
	cube_des.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	cube_des.Usage = D3D11_USAGE_DEFAULT;
	cube_des.CPUAccessFlags = 0;
	cube_des.SampleDesc.Count = 1;
	cube_des.SampleDesc.Quality = 0;
	cube_des.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;	//*
	Microsoft::WRL::ComPtr<ID3D11Texture2D> cube_texture;
	g_gfx->GetDevice()->CreateTexture2D(&cube_des, nullptr, cube_texture.GetAddressOf());
	//copy resource form file to current
	for (int i = 0; i < 6; ++i)
	{
		g_gfx->GetContext()->CopySubresourceRegion(cube_texture.Get(), D3D11CalcSubresource(0u, i, 1u)
			, 0, 0, 0, tex_vec_[i]->GetResource(), 0, nullptr);
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC rsv;
	rsv.Format = des.Format;
	rsv.TextureCube.MipLevels = 1;
	rsv.TextureCube.MostDetailedMip = 0;
	rsv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;	//*
	g_gfx->GetDevice()->CreateShaderResourceView(cube_texture.Get(), &rsv, p_srv_.GetAddressOf());
	AddBind(std::make_unique<ShaderResource>(p_srv_.Get(), ETextureType::kDiffuse));
}

void SkyBox::TexturedFromEquirectangular()
{
	//HDR 
	stbi_set_flip_vertically_on_load(true);
	int width, height, channels;
	//float* pixel = stbi_loadf("C:/Users/BoomBac/Downloads/Desert_Highway/Desert_Highway/Road_to_MonumentValley_Ref.hdr", &width, &height, &channels, 0);
	float* pixel = stbi_loadf("Y:/Project_VS2019/DX11RenderEngine/Res/Texture/Apartment_Reflection.hdr", &width, &height, &channels, STBI_rgb_alpha);
	D3D11_TEXTURE2D_DESC des{};
	des.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	des.Width = width;
	des.Height = height;
	des.ArraySize = 1;
	des.MipLevels = 1;
	des.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	des.Usage = D3D11_USAGE_DEFAULT;
	des.CPUAccessFlags = 0;
	des.SampleDesc.Count = 1;
	des.SampleDesc.Quality = 0;
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = pixel;
	data.SysMemPitch = width * sizeof(float) * 4;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> hdr_texture;
	g_gfx->GetDevice()->CreateTexture2D(&des, &data, hdr_texture.GetAddressOf());
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
	srvd.Format = des.Format;
	srvd.Texture2D.MipLevels = 1;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	g_gfx->GetDevice()->CreateShaderResourceView(hdr_texture.Get(), &srvd, p_hdr_srv_.GetAddressOf());
	AddBind(std::make_unique<ShaderResource>(p_hdr_srv_.Get(), ETextureType::kNormal));
	stbi_image_free(pixel);
}
