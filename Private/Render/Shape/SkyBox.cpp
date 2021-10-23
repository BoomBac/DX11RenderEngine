
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
	AddBind(std::move(ps));
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

	width_ = 1024;
	height_ = 1024;
	face_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	face_desc.Width = width_;
	face_desc.Height = height_;
	face_desc.ArraySize = 1;
	face_desc.MipLevels = 1;
	face_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	face_desc.Usage = D3D11_USAGE_DEFAULT;
	face_desc.CPUAccessFlags = 0;
	face_desc.SampleDesc.Count = 1;
	face_desc.SampleDesc.Quality = 0;

	ShaderingFromFile("Y:/Project_VS2019/DX11RenderEngine/Res/Texture/Apartment_Reflection.hdr");
	GenerateCubeMap(width_, height_);
	ShaderingFromResource();
}

ID3D11ShaderResourceView* SkyBox::GetEnvironmentRsv() const
{
	return p_srv_.Get();
}

void SkyBox::GenerateCubeMap(const UINT& width, const UINT& height, const std::vector<std::string>& path_list)
{
	if (path_list.size() < 6) return;
	image_paths_ = path_list;
	UINT w = g_gfx->GetWidth();
	UINT h = g_gfx->GetHeight();
	g_gfx->ResizeBackbuffer(width, height);
	Camera* cur_camera = g_gfx->p_camera_;
	Camera sample_camera;
	sample_camera.SetLocation(0.f, 0.f, 0.f);
	sample_camera.SetProjection(90.f, static_cast<float>(width) / static_cast<float>(height), 1.f, 1000.f);
	std::unique_ptr<RenderToTexture> rtt = std::make_unique<RenderToTexture>();
	rtt->Initialize(g_gfx, ERTTUsage::kBackBuffer);
	rtt->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	rtt->SetRenderTarget(g_gfx);
	g_gfx->isRenderShaodw = false;
	g_gfx->p_camera_ = &sample_camera;
	Draw(*g_gfx);
	rtt->SaveToImage(g_gfx, path_list[0]);
	sample_camera.SetRotation(DegToRad(0.f), DegToRad(180.f), DegToRad(0.f));
	Draw(*g_gfx);
	rtt->SaveToImage(g_gfx, path_list[1]);
	rtt->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	sample_camera.SetRotation(DegToRad(0.f), DegToRad(-90.f), DegToRad(0.f));
	Draw(*g_gfx);
	rtt->SaveToImage(g_gfx, path_list[2]);
	rtt->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	sample_camera.SetRotation(DegToRad(0.f), DegToRad(90.f), DegToRad(0.f));
	Draw(*g_gfx);
	rtt->SaveToImage(g_gfx, path_list[3]);
	rtt->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	sample_camera.SetRotation(DegToRad(-90.f), DegToRad(0.f), DegToRad(0.f));
	Draw(*g_gfx);
	rtt->SaveToImage(g_gfx, path_list[4]);
	rtt->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	sample_camera.SetRotation(DegToRad(90.f), DegToRad(0.f), DegToRad(0.f));
	Draw(*g_gfx);
	rtt->SaveToImage(g_gfx, path_list[5]);
	rtt->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	g_gfx->isRenderShaodw = true;
	g_gfx->p_camera_ = cur_camera;
	g_gfx->ResizeBackbuffer(w, h);
}

void SkyBox::GenerateCubeMap(const UINT& width, const UINT& height)
{
	UINT w = g_gfx->GetWidth();
	UINT h = g_gfx->GetHeight();
	g_gfx->ResizeBackbuffer(width, height);
	Camera* cur_camera = g_gfx->p_camera_;
	Camera sample_camera;
	sample_camera.SetLocation(0.f, 0.f, 0.f);
	sample_camera.SetProjection(90.f, static_cast<float>(width) / static_cast<float>(height), 1.f, 1000.f);

	D3D11_TEXTURE2D_DESC des{};
	des.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	des.Width = width;
	des.Height = height;
	des.ArraySize = 1;
	des.MipLevels = 1;
	des.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	des.Usage = D3D11_USAGE_DEFAULT;
	des.CPUAccessFlags = 0;
	des.SampleDesc.Count = 1;
	des.SampleDesc.Quality = 0;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> t_tex_front;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> t_tex_back;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> t_tex_left;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> t_tex_right;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> t_tex_top;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> t_tex_bottom;
	texture_res_.push_back(t_tex_bottom);
	texture_res_.push_back(t_tex_top);
	texture_res_.push_back(t_tex_right);
	texture_res_.push_back(t_tex_left);
	texture_res_.push_back(t_tex_back);
	texture_res_.push_back(t_tex_front);
	g_gfx->GetDevice()->CreateTexture2D(&des, NULL, texture_res_[0].GetAddressOf());
	g_gfx->GetDevice()->CreateTexture2D(&des, NULL, texture_res_[1].GetAddressOf());
	g_gfx->GetDevice()->CreateTexture2D(&des, NULL, texture_res_[2].GetAddressOf());
	g_gfx->GetDevice()->CreateTexture2D(&des, NULL, texture_res_[3].GetAddressOf());
	g_gfx->GetDevice()->CreateTexture2D(&des, NULL, texture_res_[4].GetAddressOf());
	g_gfx->GetDevice()->CreateTexture2D(&des, NULL, texture_res_[5].GetAddressOf());
	std::unique_ptr<RenderToTexture> rtt = std::make_unique<RenderToTexture>();
	rtt->Initialize(g_gfx, ERTTUsage::kBackBuffer);
	rtt->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	rtt->SetRenderTarget(g_gfx);
	g_gfx->isRenderShaodw = false;
	g_gfx->p_camera_ = &sample_camera;
	Draw(*g_gfx);
	g_gfx->GetContext()->CopyResource(texture_res_[4].Get(), rtt->GetResource());
	sample_camera.SetRotation(DegToRad(0.f), DegToRad(180.f), DegToRad(0.f));
	Draw(*g_gfx);
	g_gfx->GetContext()->CopyResource(texture_res_[5].Get(), rtt->GetResource());
	rtt->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	sample_camera.SetRotation(DegToRad(0.f), DegToRad(-90.f), DegToRad(0.f));
	Draw(*g_gfx);
	g_gfx->GetContext()->CopyResource(texture_res_[1].Get(), rtt->GetResource());
	rtt->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	sample_camera.SetRotation(DegToRad(0.f), DegToRad(90.f), DegToRad(0.f));
	Draw(*g_gfx);
	g_gfx->GetContext()->CopyResource(texture_res_[0].Get(), rtt->GetResource());
	rtt->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	sample_camera.SetRotation(DegToRad(-90.f), DegToRad(0.f), DegToRad(0.f));
	Draw(*g_gfx);
	g_gfx->GetContext()->CopyResource(texture_res_[2].Get(), rtt->GetResource());
	rtt->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	sample_camera.SetRotation(DegToRad(90.f), DegToRad(0.f), DegToRad(0.f));
	Draw(*g_gfx);
	g_gfx->GetContext()->CopyResource(texture_res_[3].Get(), rtt->GetResource());
	rtt->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	g_gfx->isRenderShaodw = true;
	g_gfx->p_camera_ = cur_camera;
	g_gfx->ResizeBackbuffer(w,h);
}

void SkyBox::ShaderingFromResource()
{
	//create cube desc and texture2d
	D3D11_TEXTURE2D_DESC cube_des{};
	cube_des.Format = face_desc.Format;
	cube_des.Width = face_desc.Width;
	cube_des.Height = face_desc.Height;
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
	for (int i = 0; i < 6; ++i)
	{
		g_gfx->GetContext()->CopySubresourceRegion(cube_texture.Get(), D3D11CalcSubresource(0u, i, 1u)
			, 0, 0, 0, texture_res_[i].Get(), 0, nullptr);
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC rsv;
	rsv.Format = face_desc.Format;
	rsv.TextureCube.MipLevels = 1;
	rsv.TextureCube.MostDetailedMip = 0;
	rsv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;	//*
	g_gfx->GetDevice()->CreateShaderResourceView(cube_texture.Get(), &rsv, p_srv_.GetAddressOf());
	for (auto it = binds.begin(); it != binds.end(); it++)
	{
		if ((*it)->GetType() == EBindableType::kPixelShader)
		{
			(*it).reset(new PixelShader(*g_gfx, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/pSkyBoxC.cso"));
			continue;
		}
		if ((*it)->GetType() == EBindableType::kShaderResource)
		{
			it = binds.erase(it);
			if (it == binds.end()) break;
		}
	}
	AddBind(std::make_unique<ShaderResource>(p_srv_.Get(), ETextureType::kDiffuse));
}

void SkyBox::ShaderingFromFile(const std::string& sphere_image_path)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, channels;
	float* pixel = stbi_loadf(sphere_image_path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
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
	AddBind(std::make_unique<ShaderResource>(p_hdr_srv_.Get(), ETextureType::kDiffuse));
	stbi_image_free(pixel);
}

//TODO : adapted file format
void SkyBox::ShaderingFromFile(const std::vector<std::string>& cube_image_paths, const UINT& width, const UINT& height)
{
	D3D11_TEXTURE2D_DESC cube_des{};
	cube_des.Format = face_desc.Format;
	cube_des.Width = width;
	cube_des.Height = height;
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
	Texture tex_top(g_gfx->GetDevice(), cube_image_paths[0].c_str(), face_desc);
	Texture tex_bottom(g_gfx->GetDevice(), cube_image_paths[1].c_str(), face_desc);
	Texture tex_right(g_gfx->GetDevice(), cube_image_paths[2].c_str(), face_desc);
	Texture tex_left(g_gfx->GetDevice(), cube_image_paths[3].c_str(), face_desc);
	Texture tex_front(g_gfx->GetDevice(), cube_image_paths[4].c_str(), face_desc);
	Texture tex_back(g_gfx->GetDevice(), cube_image_paths[5].c_str(), face_desc);
	g_gfx->GetContext()->CopySubresourceRegion(cube_texture.Get(), D3D11CalcSubresource(0u, 0, 1u), 0, 0, 0, tex_top.GetResource(), 0, nullptr);
	g_gfx->GetContext()->CopySubresourceRegion(cube_texture.Get(), D3D11CalcSubresource(0u, 1, 1u), 0, 0, 0, tex_bottom.GetResource(), 0, nullptr);
	g_gfx->GetContext()->CopySubresourceRegion(cube_texture.Get(), D3D11CalcSubresource(0u, 2, 1u), 0, 0, 0, tex_right.GetResource(), 0, nullptr);
	g_gfx->GetContext()->CopySubresourceRegion(cube_texture.Get(), D3D11CalcSubresource(0u, 3, 1u), 0, 0, 0, tex_left.GetResource(), 0, nullptr);
	g_gfx->GetContext()->CopySubresourceRegion(cube_texture.Get(), D3D11CalcSubresource(0u, 4, 1u), 0, 0, 0, tex_front.GetResource(), 0, nullptr);
	g_gfx->GetContext()->CopySubresourceRegion(cube_texture.Get(), D3D11CalcSubresource(0u, 5, 1u), 0, 0, 0, tex_back.GetResource(), 0, nullptr);
	D3D11_SHADER_RESOURCE_VIEW_DESC rsv;
	rsv.Format = face_desc.Format;
	rsv.TextureCube.MipLevels = 1;
	rsv.TextureCube.MostDetailedMip = 0;
	rsv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;	//*
	g_gfx->GetDevice()->CreateShaderResourceView(cube_texture.Get(), &rsv, p_srv_.GetAddressOf());
	for (auto it = binds.begin(); it != binds.end(); it++)
	{
		if ((*it)->GetType() == EBindableType::kPixelShader)
		{
			(*it).reset(new PixelShader(*g_gfx, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/pSkyBoxC.cso"));
			continue;
		}
		if ((*it)->GetType() == EBindableType::kShaderResource)
		{
			it = binds.erase(it);
			if (it == binds.end()) break;
		}
	}
	AddBind(std::make_unique<ShaderResource>(p_srv_.Get(), ETextureType::kDiffuse));
}
