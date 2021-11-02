
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
#include <Public/Render/Bindable/LightBuffer.h>
#include <Public/Render/Material/Material.h>





template<typename T>
using Vec = std::vector<T, std::allocator<T>>;
using BindItem = std::unique_ptr<BindableInterface>;

namespace
{
	static Graphics* g_gfx;
	std::vector<float> g_six_viws{
		0.f,DegToRad(90.f),0.f,	//+x
		0.f,DegToRad(-90.f),0.f,	// -x
		DegToRad(-90.f),0.f,0.f,	//+y
		DegToRad(90.f),0.f,0.f,	//-y
		0.f,0.f,0.f,		//+z
		0.f,DegToRad(180.f),0.f,	//-z
	};
}


SkyBox::SkyBox(Graphics& gfx)
{
	InitBindable(gfx);
	effects_.push_back(EEffectType::kNone);
	image_paths_.push_back("C:/Users/BoomBac/Desktop/top.png");
	image_paths_.push_back("C:/Users/BoomBac/Desktop/bottom.png");
	image_paths_.push_back("C:/Users/BoomBac/Desktop/right.png");
	image_paths_.push_back("C:/Users/BoomBac/Desktop/left.png");
	image_paths_.push_back("C:/Users/BoomBac/Desktop/front.png");
	image_paths_.push_back("C:/Users/BoomBac/Desktop/back.png");

	environment_map_size_ = 1024;
	irradiance_map_size_ = 1;
	face_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	face_desc.Width = environment_map_size_;
	face_desc.Height = environment_map_size_;
	face_desc.ArraySize = 1;
	face_desc.MipLevels = 6;
	face_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	face_desc.Usage = D3D11_USAGE_DEFAULT;
	face_desc.CPUAccessFlags = 0;
	face_desc.SampleDesc.Count = 1;
	face_desc.SampleDesc.Quality = 0;

	//GenerateLUT(std::string{});

	//ShaderingFromFile("Y:/Project_VS2019/DX11RenderEngine/Res/Texture/Apartment_Reflection.hdr");
	ShaderingFromFile("C:/Users/BoomBac/Downloads/Desert_Highway/Desert_Highway/Road_to_MonumentValley_Ref.hdr");
	GenerateCubeSurface(environment_map_size_,EGenerateFlag::kEnvironment);
	GenerateCube(EGenerateFlag::kEnvironment);
	ShaderingFromResource(EGenerateFlag::kEnvironment);
	GenerateCubeSurface(environment_map_size_, EGenerateFlag::kSpecular);
	GenerateIrrandiceSurface(irradiance_map_size_);
	GenerateCube(EGenerateFlag::kIrradiance);
	GenerateCube(EGenerateFlag::kSpecular);
	//ShaderingFromResource(EGenerateFlag::kSpecular);
	
	//GenerateCube(EGenerateFlag::kSpecular);
	//ShaderingFromResource(EGenerateFlag::kSpecular);
	//ShaderingFromResource(EGenerateFlag::kIrradiance);
	LoadLUT("C:/Users/BoomBac/Desktop/ibl_brdf_lut.png");
	BindToShadering(EGenerateFlag::kSpecular);
	BindToShadering(EGenerateFlag::kIrradiance);
	ShaderingFromResource(EGenerateFlag::kEnvironment);
}

ID3D11ShaderResourceView* SkyBox::GetEnvironmentRsv() const
{
	return p_env_cube_.Get();
}

void SkyBox::GenerateCubeSurface(const UINT& size, const std::vector<std::string>& path_list)
{
	if (path_list.size() < 6) return;
	environment_map_size_ = size;
	image_paths_ = path_list;
	UINT w = g_gfx->GetWidth();
	UINT h = g_gfx->GetHeight();
	g_gfx->ResizeBackbuffer(environment_map_size_, environment_map_size_);
	Camera* cur_camera = g_gfx->p_camera_;
	Camera sample_camera;
	sample_camera.SetLocation(0.f, 0.f, 0.f);
	sample_camera.SetProjection(90.f, 1.f, 1.f, 1000.f);
	std::unique_ptr<RenderToTexture> rtt_depth = std::make_unique<RenderToTexture>();
	rtt_depth->Initialize(g_gfx, ERTTUsage::kBackBuffer,face_desc);
	rtt_depth->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	rtt_depth->SetRenderTarget(g_gfx);
	g_gfx->p_camera_ = &sample_camera;
	g_gfx->UpdatePerFrameBuf(std::string("gViewProj"), sample_camera.view_projection_matrix());
	Draw(*g_gfx);
	rtt_depth->SaveToImage(g_gfx, path_list[0]);
	sample_camera.SetRotation(DegToRad(0.f), DegToRad(180.f), DegToRad(0.f));
	g_gfx->UpdatePerFrameBuf(std::string("gViewProj"), sample_camera.view_projection_matrix());
	Draw(*g_gfx);
	rtt_depth->SaveToImage(g_gfx, path_list[1]);
	rtt_depth->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	sample_camera.SetRotation(DegToRad(0.f), DegToRad(-90.f), DegToRad(0.f));
	g_gfx->UpdatePerFrameBuf(std::string("gViewProj"), sample_camera.view_projection_matrix());
	Draw(*g_gfx);
	rtt_depth->SaveToImage(g_gfx, path_list[2]);
	rtt_depth->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	sample_camera.SetRotation(DegToRad(0.f), DegToRad(90.f), DegToRad(0.f));
	g_gfx->UpdatePerFrameBuf(std::string("gViewProj"), sample_camera.view_projection_matrix());
	Draw(*g_gfx);
	rtt_depth->SaveToImage(g_gfx, path_list[3]);
	rtt_depth->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	sample_camera.SetRotation(DegToRad(-90.f), DegToRad(0.f), DegToRad(0.f));
	g_gfx->UpdatePerFrameBuf(std::string("gViewProj"), sample_camera.view_projection_matrix());
	Draw(*g_gfx);
	rtt_depth->SaveToImage(g_gfx, path_list[4]);
	rtt_depth->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	sample_camera.SetRotation(DegToRad(90.f), DegToRad(0.f), DegToRad(0.f));
	g_gfx->UpdatePerFrameBuf(std::string("gViewProj"), sample_camera.view_projection_matrix());
	Draw(*g_gfx);
	rtt_depth->SaveToImage(g_gfx, path_list[5]);
	rtt_depth->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	g_gfx->p_camera_ = cur_camera;
	g_gfx->ResizeBackbuffer(w, h);
}

void SkyBox::GenerateCubeSurface(const UINT& size, EGenerateFlag map_type)
{
	environment_map_size_ = size;
	UINT w = g_gfx->GetWidth();
	UINT h = g_gfx->GetHeight();
	g_gfx->ResizeBackbuffer(size, size);
	Camera* cur_camera = g_gfx->p_camera_;
	Camera sample_camera;
	sample_camera.SetLocation(0.f, 0.f, 0.f);
	sample_camera.SetProjection(90.f, 1.f, 1.f, 1000.f);

	D3D11_TEXTURE2D_DESC des{};
	des.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	des.Width = size;
	des.Height = size;
	des.ArraySize = 1;
	des.MipLevels = cube_map_level;
	des.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	des.Usage = D3D11_USAGE_DEFAULT;
	des.CPUAccessFlags = 0;
	des.SampleDesc.Count = 1;
	des.SampleDesc.Quality = 0;
	des.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> env_faces[6];
	std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>>* output;
	if (map_type == EGenerateFlag::kEnvironment)
	{
		output = &texture_res_;
		for (auto& b : binds)
		{
			if (b->GetType() == EBindableType::kPixelShader)
			{
				b.reset(new PixelShader(*g_gfx, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/pSkyBox.cso"));
			}
		}
	}
	else if (map_type == EGenerateFlag::kSpecular)
	{
		output = &specular_res_;
		des.ArraySize = max_roughness_level_;
		for (auto& b : binds)
		{
			if (b->GetType() == EBindableType::kPixelShader)
			{
				b.reset(new PixelShader(*g_gfx, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/pSkyBoxS.cso"));
			}
		}
	}
	for (int i = 0; i < 6; ++i)
	{
		output->push_back(env_faces[i]);
		g_gfx->GetDevice()->CreateTexture2D(&des, NULL, (*output)[i].GetAddressOf());
	}
	std::unique_ptr<RenderToTexture> rtt_depth = std::make_unique<RenderToTexture>();
	rtt_depth->Initialize(g_gfx, ERTTUsage::kBackBuffer,face_desc);
	rtt_depth->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
	rtt_depth->SetRenderTarget(g_gfx);
	g_gfx->isRenderShaodw = false;
	g_gfx->p_camera_ = &sample_camera;

	if (map_type == EGenerateFlag::kEnvironment)
	{
		for (int i = 0; i < 6; i++)
		{
			rtt_depth->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
			int ii = i * 3;
			sample_camera.SetRotation(g_six_viws[ii], g_six_viws[ii + 1], g_six_viws[ii + 2]);
			g_gfx->UpdatePerFrameBuf(std::string("gViewProj"), sample_camera.view_projection_matrix());
			Draw(*g_gfx);
			g_gfx->GetContext()->CopySubresourceRegion((*output)[i].Get(), 0, 0u, 0u, 0u, rtt_depth->GetResource(), 0, nullptr);
		}
		//generate mip-map
		//generate mipmap for the prefilter
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_env_faces[6];
		D3D11_SHADER_RESOURCE_VIEW_DESC rsv;
		rsv.Format = face_desc.Format;
		rsv.Texture2D.MipLevels = cube_map_level;
		rsv.Texture2D.MostDetailedMip = 0;
		rsv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;	//*
		for (int i = 0; i < 6; ++i)
		{
			g_gfx->GetDevice()->CreateShaderResourceView((*output)[i].Get(), &rsv, srv_env_faces[i].GetAddressOf());
			g_gfx->GetContext()->GenerateMips(srv_env_faces[i].Get());
		}
	}
	else if (map_type == EGenerateFlag::kSpecular)
	{
		Material mat;
		mat.LoadFromLib("pSkyBoxS.cso");
		for (int r_level = 0; r_level < max_roughness_level_; ++r_level)
		{
			mat.SetFloat("roughness", static_cast<float>(r_level) / static_cast<float>(max_roughness_level_ - 1));
			//roughness_.roughness = ;
			auto s = size / static_cast<UINT>(pow(2, r_level));
			g_gfx->ResizeBackbuffer(s,s);
			//rtt->ReleaseResource();
			des.Width = s;
			des.Height = s;
			rtt_depth->Initialize(g_gfx, ERTTUsage::kBackBuffer, des);
			rtt_depth->SetRenderTarget(g_gfx);
			for (int sur_count = 0; sur_count < 6; ++sur_count)
			{
				rtt_depth->ClearRenderTarget(g_gfx, 0.f, 1.f, 1.f, 1.f);
				int ii = sur_count * 3;
				sample_camera.SetProjection(90.f, 1.f, 1.f, 1000.f);
				sample_camera.SetRotation(g_six_viws[ii], g_six_viws[ii + 1], g_six_viws[ii + 2]);
				g_gfx->UpdatePerFrameBuf(std::string("gViewProj"), sample_camera.view_projection_matrix());
				mat.CommitAllBufferData();
				Draw(*g_gfx);
				g_gfx->GetContext()->CopySubresourceRegion((*output)[sur_count].Get(), 
					D3D11CalcSubresource(r_level, 0u,max_roughness_level_), 0u, 0u, 0u, rtt_depth->GetResource(), 0, nullptr);
			}
		}
	}
	//recovery graphics property
	g_gfx->isRenderShaodw = true;
	g_gfx->p_camera_ = cur_camera;
	g_gfx->ResizeBackbuffer(w, h);
}

void SkyBox::GenerateSpecularSurface()
{
}

void SkyBox::GenerateIrrandiceSurface(const UINT& size)
{
	if (size == environment_map_size_) return;
	UINT num = environment_map_size_ / size;
	UINT level = 0;
	while (true)
	{
		num /= 2;
		if ( num != 0)
			++level;
		else break;
	}

	D3D11_TEXTURE2D_DESC des{};
	texture_res_[0]->GetDesc(&des);
	des.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	des.Width = size;
	des.Height = size;
	des.ArraySize = 1;
	des.MipLevels = 1;
	des.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	des.Usage = D3D11_USAGE_DEFAULT;
	des.CPUAccessFlags = 0;
	des.SampleDesc.Count = 1;
	des.SampleDesc.Quality = 0;
	des.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA sdata{};
	Microsoft::WRL::ComPtr<ID3D11Texture2D> r;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> l;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> t;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> bt;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> f;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> ba;
	irradiance_res_.push_back(r);
	irradiance_res_.push_back(l);
	irradiance_res_.push_back(t);
	irradiance_res_.push_back(bt);
	irradiance_res_.push_back(f);
	irradiance_res_.push_back(ba);
	for (int i = 0; i < 6; ++i)
	{
		g_gfx->GetDevice()->CreateTexture2D(&des, nullptr, irradiance_res_[i].GetAddressOf());
		g_gfx->GetContext()->CopySubresourceRegion(irradiance_res_[i].Get(), 0u, 0u, 0u, 0u, texture_res_[i].Get(),level, nullptr);
	}
}

void SkyBox::GenerateSpecularCube()
{

}

void SkyBox::ShaderingFromResource(EGenerateFlag map_type)
{
	for (auto it = binds.begin(); it != binds.end(); it++)
	{
		if ((*it)->GetType() == EBindableType::kPixelShader)
		{
			(*it).reset(new PixelShader(*g_gfx, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/pSkyBoxC.cso"));
			continue;
		}
		if ((*it)->GetType() == EBindableType::kShaderResource)
		{
			if (dynamic_cast<ShaderResource*>((*it).get())->GetTextureType() == ESResourceType::kDiffuse)
			{
				if (map_type == EGenerateFlag::kEnvironment)
				{
					(*it).reset(new ShaderResource(p_env_cube_.Get(), ESResourceType::kDiffuse));
				}
				else if (map_type == EGenerateFlag::kIrradiance)
				{
					(*it).reset(new ShaderResource(p_hdr_cube_.Get(), ESResourceType::kDiffuse));
				}
				else if (map_type == EGenerateFlag::kSpecular)
				{
					(*it).reset(new ShaderResource(p_specular_cube_.Get(), ESResourceType::kDiffuse));
				}
			}
		}
	}
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
	g_gfx->GetDevice()->CreateShaderResourceView(hdr_texture.Get(), &srvd, p_hdr_cube_.GetAddressOf());
	AddBind(std::make_unique<ShaderResource>(p_hdr_cube_.Get(), ESResourceType::kDiffuse));
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
	g_gfx->GetDevice()->CreateShaderResourceView(cube_texture.Get(), &rsv, p_env_cube_.GetAddressOf());
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
	AddBind(std::make_unique<ShaderResource>(p_env_cube_.Get(), ESResourceType::kDiffuse));
}

void SkyBox::BindToShadering(EGenerateFlag env_type)
{
	if(env_type == EGenerateFlag::kIrradiance)
		AddBind(std::make_unique<ShaderResource>(p_hdr_cube_.Get(), ESResourceType::kIrradiance));
	else if (env_type == EGenerateFlag::kSpecular)
	{
		AddBind(std::make_unique<ShaderResource>(p_specular_cube_.Get(), ESResourceType::kSpecularMap));
		AddBind(std::make_unique<ShaderResource>(p_LUT_map_.Get(), ESResourceType::kLUT));
	}
	else if(env_type == EGenerateFlag::kEnvironment)
		AddBind(std::make_unique<ShaderResource>(p_env_cube_.Get(), ESResourceType::kIrradiance));
}

void SkyBox::LoadLUT(const std::string& path)
{
	Texture LUT(g_gfx->GetDevice(), path.c_str());
	p_LUT_map_ = LUT.GetTextureResourceView();
}

void SkyBox::InitBindable(Graphics& gfx)
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
	v_cons_buf_.world_matrix_ = DirectX::XMMatrixTranslation(initPos.x, initPos.y, initPos.z);
	BindItem vcb = std::make_unique<TransformBuffer>(gfx, *this);
	vcb->vc_buf_index_ = 1;
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
}

void SkyBox::GenerateCube(EGenerateFlag cube_type)
{
	std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>>* p_res;
	UINT image_size;
	if (cube_type == EGenerateFlag::kEnvironment)
	{
		p_res = &texture_res_;
		image_size = face_desc.Width;
	}
	else if (cube_type == EGenerateFlag::kIrradiance)
	{
		p_res = &irradiance_res_;
		image_size = irradiance_map_size_;
	}
	else if (cube_type == EGenerateFlag::kSpecular)
	{
		p_res = &specular_res_;
		image_size = environment_map_size_;
	}
	D3D11_TEXTURE2D_DESC cube_des{};
	cube_des.Format = face_desc.Format;
	cube_des.Width = image_size;//face_desc.Width;
	cube_des.Height = image_size;//face_desc.Height;
	cube_des.ArraySize = 6;	//*
	cube_des.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	cube_des.Usage = D3D11_USAGE_DEFAULT;
	cube_des.MipLevels = cube_map_level;
	cube_des.CPUAccessFlags = 0;
	cube_des.SampleDesc.Count = 1;
	cube_des.SampleDesc.Quality = 0;
	cube_des.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> cube_texture;
	D3D11_SHADER_RESOURCE_VIEW_DESC rsv;
	rsv.Format = face_desc.Format;
	rsv.TextureCube.MostDetailedMip = 0;
	rsv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;	//*
	rsv.TextureCube.MipLevels = cube_map_level;
	if (cube_type == EGenerateFlag::kIrradiance)
	{
		rsv.TextureCube.MipLevels = 1;
		cube_des.MipLevels = 1;
	}
	g_gfx->GetDevice()->CreateTexture2D(&cube_des, nullptr, cube_texture.GetAddressOf());
	//irradance dont't create mip-map
	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < cube_des.MipLevels; ++j)
		{
			g_gfx->GetContext()->CopySubresourceRegion(cube_texture.Get(), D3D11CalcSubresource(j, i, cube_des.MipLevels)
				, 0, 0, 0, (*p_res)[i].Get(), j, nullptr);
		}
	}
	if (cube_type == EGenerateFlag::kEnvironment)
	{
		g_gfx->GetDevice()->CreateShaderResourceView(cube_texture.Get(), &rsv, p_env_cube_.GetAddressOf());
	}
	else if (cube_type == EGenerateFlag::kIrradiance)
	{
		g_gfx->GetDevice()->CreateShaderResourceView(cube_texture.Get(), &rsv, p_hdr_cube_.GetAddressOf());
	}
	else if (cube_type == EGenerateFlag::kSpecular)
	{
		g_gfx->GetDevice()->CreateShaderResourceView(cube_texture.Get(), &rsv, p_specular_cube_.GetAddressOf());
	}

}

