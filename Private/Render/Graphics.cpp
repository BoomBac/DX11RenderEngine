#include <d3d11.h>
#include <d3dcompiler.h>
#include <utility>




#include "Public/Render/Graphics.h"
#include "Public/Global.h"
#include "Public/Render/Bindable/DepthStencil.h"
#include "Public/Render/Shape/Box.h"
#include "Public/Tool/Utili.h"
#include "Public/Render/Drawable/Coordinate.h"
#include "Public/Render/GeometryFactory.h"
#include "Public/Render/ModelResFactory.h"
#include "vector2D.h"
#include <Public/Render/ResManage/MeshFactory.h>
#include <Public/Render/ResManage/TextureFactory.h>
#include "Public/Render/Light/DirectionalLight.h"
#include "Public/Render/Light/PointLight.h"
#include "Public/Render/Light/SpotLight.h"
#include "Public/Render/RenderToTexture.h"
#include "ScreenGrab.h"
#include "Public/Render/Shape/SkyBox.h"
#include "Public/Render/Bindable/SamplerState.h"
#include <Public/Render/Material/Material.h>
#include "Public/Render/Bindable/LightBuffer.h"
#include "Public/Render/Effect/Effect.h"



#pragma comment(lib,"d3d11.lib") 
#pragma comment(lib,"dxgi.lib") 
#pragma comment(lib,"D3DCompiler.lib")



template<typename T>
using Vec = std::vector<T, std::allocator<T>>;

Drawable* Graphics::p_selected_object_ = nullptr;





namespace 
{
	//scene lightSet for the pixelConstBuffer slot 0
	std::unique_ptr<PSConstantBuffer<LightSet>> g_light_info;
	std::unique_ptr<Effect> g_pbr_effect;
	std::unique_ptr<Effect> g_none_effect;
	std::unique_ptr<Effect> g_shadow_effect;
}

Graphics::Graphics(HWND hWnd)
{
	InitializeD3DBase(hWnd,800,600);

	//init Global
	Global::getInstance()->G_Device = pDevice.Get();
	Global::getInstance()->G_Context = pDeviceContext.Get();
	Global::getInstance()->G_Gfx = this;

	ResizeBackbuffer(800, 600);
	InitializeSharedBindable();

	static float color[] = { 0,0,0,1 };
	bg_color = color;
	shadowParma.light_far = 1000.f;
	shadowParma.light_near = 1.f;
	shadowParma.light_size = 0.4f;
	shadowParma.max_bias = 0.001f;
	p_shadow_effect_ = &shadowParma;

	p_camera_ = new Camera();
	p_camera_->SetProjection(75.f, 4.f / 3.f, 1.f, 1000.f);
	camera_set_.push_back(p_camera_);

	cam_move_state_ = ECameraMovementState::kStop;
	outline_notify_ = new Subject();
	//初始化工厂类
	ResManage::InitResManage(this);
	GeometryFactory geo_factory(this);

	TextureFactory::GetInstance().AddTexture("rustediron2_basecolor.png");
	TextureFactory::GetInstance().AddTexture("rustediron2_metallic.png");
	TextureFactory::GetInstance().AddTexture("rustediron2_normal.png");
	TextureFactory::GetInstance().AddTexture("rustediron2_roughness.png");

	default_light_shader_.light_color_ = { 1.f,1.f,1.f };
	default_light_shader_.light_intensity_ = 0.2f;
	p_light_shader_ = &default_light_shader_;

	MeshFactory::getInstance().AddMesh("point_light.obj");
	MeshFactory::getInstance().AddMesh("directional_light.obj");
	MeshFactory::getInstance().AddMesh("spot_light.obj");

	common_v_cbuf_src.gCamreaPos = p_camera_->location_f();
	common_v_cbuf_src.gViewProj = p_camera_->view_projection_matrix();
	common_v_cbuf_src.gView = p_camera_->view_matrix();
	//init per-frame update constbuffer for vertex stage
	p_com_vcons_buf_ = std::make_unique<VConstantBuffer<VCommonStruct>>(*this, common_v_cbuf_src);
	p_com_vcons_buf_->vc_buf_index_ = 0;
	p_com_vcons_buf_->Update(*this, common_v_cbuf_src);
	p_com_vcons_buf_->Bind(*this);
	//init per-frame update constbuffer for pixel stage
	p_com_pcons_buf_ = std::make_unique<PConstantBuffer<PCommonStruct>>(*this, common_p_cbuf_src);
	p_com_pcons_buf_->pc_buf_index = 0;
	p_com_pcons_buf_->Update(*this, common_p_cbuf_src);
	p_com_pcons_buf_->Bind(*this);

	//创建默认灯光
	AddLight(ELightType::kPonintLight);
	p_light_->SetWorldLocation({ 20.f,30.f,0.f });
	dynamic_cast<PointLight*>(p_light_)->SetRadius(100.f);
	p_light_camera = &p_light_->light_camera_;
	p_light_matrix_ = p_light_->GetLightMatrix();

	MeshFactory::getInstance().AddMesh("sphere.obj");
	MeshFactory::getInstance().AddMesh("plane.obj");

	g_light_info = std::move(std::make_unique<PSConstantBuffer<LightSet>>(*this, &p_scene_light_));

	g_pbr_effect = std::make_unique<Effect>(EEffectType::kPBREffect);
	g_none_effect = std::make_unique<Effect>(EEffectType::kNone);
	g_shadow_effect = std::make_unique<Effect>(EEffectType::kShadow);

	//初始化坐标轴和场景物体
	InitSceneObject();
}

Graphics::~Graphics()
{
	if (outline_notify_ != nullptr)
		delete outline_notify_;

	if (!camera_set_.empty())
	{
		for (auto &i : camera_set_)
		{
			delete i;
		}
	}
	if (!scene_objects_.empty())
	{
		for (auto &i : scene_objects_)
		{
			delete i;
		}
	}
}

void Graphics::EndFrame()
{
	p_light_->UpdateAttribute();
	UpdatePerFrameBuf();
	//update light_const_buffer at slot 0
	//g_light_info->pc_buf_index = 0;
	//g_light_info->Bind(*this);
	//clear rtv and d&s
	pDeviceContext->ClearRenderTargetView(p_render_targetview_.Get(), bg_color);
	p_depth_stencil_->Clear(*this);
	//p_sky_box_->GenerateCubeSurface(1024u, EGenerateFlag::kSpecular);
	//GetContext()->OMSetRenderTargets(1, p_render_targetview_.GetAddressOf(), p_depth_stencil_->pDepthStencilView.Get());
	
	g_pbr_effect->Render(effect_bucket_.find(EEffectType::kPBREffect)->second);
	g_shadow_effect->Render(effect_bucket_.find(EEffectType::kShadow)->second);
	g_none_effect->Render(effect_bucket_.find(EEffectType::kNone)->second);


	p_sky_box_->Draw(*this);
	p_coordinate_->Draw(*this);

	UpdateCameraMovement();
	pSwapChain->Present(0u, 0u);
}

void Graphics::DrawIndexed(const UINT& count)
{
	pDeviceContext->DrawIndexed(count, 0u, 0u);
}


void Graphics::SetVPBackColor(float color[4])
{
	bg_color = color;
}

void Graphics::CommitPerFrameBuf()
{
	p_com_vcons_buf_->Update(*this, common_v_cbuf_src);
	p_com_vcons_buf_->Bind(*this);
	p_com_pcons_buf_->Update(*this, common_p_cbuf_src);
	p_com_pcons_buf_->Bind(*this);
}

void Graphics::UpdatePerFrameBuf(std::string& val_name, const DirectX::XMMATRIX& mat)
{
	if (!val_name.compare("gProj"))
	{
		common_v_cbuf_src.gProj = mat;
	}
	else if(!val_name.compare("gView"))
	{
		common_v_cbuf_src.gView = mat;
	}
	else if (!val_name.compare("gViewProj"))
	{
		common_v_cbuf_src.gViewProj = mat;
	}
	CommitPerFrameBuf();
}

void Graphics::UpdatePerFrameBuf(std::string& val_name, const DirectX::XMFLOAT3& f3)
{
	if (!val_name.compare("gCamreaPos"))
	{
		common_v_cbuf_src.gCamreaPos = f3;
	}
	CommitPerFrameBuf();
}

void Graphics::SetSelectedObjectTranslate(const CusMath::vector3d& t)
{
	if (p_coordinate_->GetCoordinateType())
	{
		p_selected_object_->SetWorldLocation(t);
	}
	else
	{
		p_selected_object_->SetActorLocation(t);
	}
}


void Graphics::SetSelectedObjectRotation(const CusMath::vector3d& t)
{
	if (p_coordinate_->GetCoordinateType())
	{
		p_selected_object_->SetWorldRotation(t);
	}
	else
	{
		p_selected_object_->SetActorRotation(t);
	}
}

void Graphics::SetSelectedObjectScale(const CusMath::vector3d& t)
{
	p_selected_object_->SetActorScale(t);
}

void Graphics::AddSceneObject(Drawable* object, std::string object_name)
{
	scene_objects_.push_back(object);
	scene_outline_.insert(std::pair<int, std::string>(scene_objects_.size() - 1, object_name));
	last_add_object_name_ = object_name;
	object->SetName(object_name);
	outline_notify_->NotifyObserver(true);
	AdjustRenderQueue();
}


void Graphics::DeleteSceneObject(int index)
{
	auto it = scene_objects_.begin();
	it += index;
	delete *it;
	scene_objects_.erase(it);
	AdjustRenderQueue();
}

void Graphics::InitSceneObject()
{
	//创建坐标轴
	p_coordinate_ = std::make_unique<Coordinate>(*this, 10.f);
	auto sphere = GeometryFactory::GenerateGeometry("sphere.obj");
	sphere->AddEffect(EEffectType::kShadow);
	auto plane = GeometryFactory::GenerateGeometry("plane.obj");
	plane->AddEffect(EEffectType::kShadow);
	//GeometryFactory::GenerateGeometry("cubbe.obj");
	plane->SetActorScale({ 5.f,1.f,5.f });
	p_sky_box_ = std::make_unique<SkyBox>(*this);
	//set skybox as environment_map at slot 6
	auto p = p_sky_box_->GetEnvironmentRsv();
	pDeviceContext->PSSetShaderResources(6, 1, &p);
	AdjustRenderQueue();
	SetSelectObject(1);
}

int Graphics::InitOutline(std::string* item_name)
{
	int i = 0;
	for (auto object : scene_outline_)
	{
		item_name[i] = object.second; ++i;
	}

	return i;
}

void Graphics::AddLight(ELightType light_type)
{
	switch (light_type)
	{
	case ELightType::kPonintLight:
	{
		p_light_ = new PointLight(*this);
		AddSceneObject(p_light_, "PointLight");
	}
	break;
	case ELightType::kDirectionLight:
	{
		p_light_ = new DirectionalLight(*this);
		AddSceneObject(p_light_, "DirectionalLight");
	}
	break;
	case ELightType::kSpotLight:
	{
		p_light_ = new SpotLight(*this);
		AddSceneObject(p_light_, "SpotLight");
	}
	break;
	default:
		break;
	}
}

void Graphics::SetSelectObject(const int& index)
{
	if (index != -1)
	{
		SetSelectObject(scene_objects_[index]);
		if (dynamic_cast<Light*>(p_selected_object_))
		{
			p_light_ = dynamic_cast<Light*>(p_selected_object_);
			p_camera_ = &dynamic_cast<Light*>(p_light_)->light_camera_;
			p_scene_light_ = p_light_->GetAttritute();
			p_light_camera = &p_light_->light_camera_;
			p_light_matrix_ = p_light_->GetLightMatrix();
			//p_light_view_projection_ = &dynamic_cast<Light*>(p_light_)->GetLightMatrix()->view_porjection;
			//p_light_view_projection_ = dynamic_cast<Light*>(p_light_)->light_camera_.view_projection_matrix();
		}
		else
		{
			p_camera_ = camera_set_[0];
		}
	}
	else
	SetSelectObject(nullptr);
}
void Graphics::ResizeBackbuffer(const UINT& w, const UINT& h)
{
	static bool isInitialize = false;
	if (width_ == w && height_ == h && isInitialize == true) return;
	width_ = w;
	height_ = h;
	if (!isInitialize)
	{
		ID3D11Texture2D* backBuffer = nullptr;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
		if (backBuffer != nullptr)
			pDevice->CreateRenderTargetView(backBuffer, NULL, p_render_targetview_.GetAddressOf());
		backBuffer->Release();
		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = width_;
		vp.Height = height_;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		pDeviceContext->RSSetViewports(1, &vp);
		isInitialize = true;
	}
	else
	{
		ID3D11RenderTargetView* null_view[] = { nullptr };
		pDeviceContext->OMSetRenderTargets(sizeof(null_view) / sizeof(void*), null_view, nullptr);
		p_render_targetview_->Release();
		auto hr = pSwapChain->ResizeBuffers(1, w, h, /*DXGI_FORMAT_R16G16B16A16_FLOAT*/DXGI_FORMAT_UNKNOWN, 0);
		ID3D11Texture2D* pBuffer = nullptr;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBuffer));
		if(pBuffer!=nullptr)
			pDevice->CreateRenderTargetView(pBuffer, NULL, p_render_targetview_.GetAddressOf());
		pBuffer->Release();
		p_depth_stencil_.reset(new DepthStencil(w,h,*this));
		pDeviceContext->OMSetRenderTargets(1, p_render_targetview_.GetAddressOf(), p_depth_stencil_view_.Get());
		// Set up the viewport.
		D3D11_VIEWPORT vp;
		vp.Width = w;
		vp.Height = h;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		pDeviceContext->RSSetViewports(1, &vp);
		p_camera_->SetProjection(75.f, static_cast<float>(w) / static_cast<float>(h), 1.f, 1000.f);
		if (hr == S_OK)
		{
			qDebug() << "current size is" << w << "," << h;
		}
	}

}
void Graphics::SetSelectObject(Drawable* object)
{
	p_selected_object_ = object;
	p_coordinate_->SetAttachedObject(p_selected_object_);
}

void Graphics::AdjustRenderQueue()
{
	static bool b_init = false;
	if (!b_init)
	{
		effect_bucket_.insert(std::make_pair(EEffectType::kNone, std::vector<Drawable*>{}));
		effect_bucket_.insert(std::make_pair(EEffectType::kPBREffect, std::vector<Drawable*>{}));
		effect_bucket_.insert(std::make_pair(EEffectType::kShadow, std::vector<Drawable*>{}));
		b_init = true;
	}
	else
	{
		effect_bucket_.find(EEffectType::kNone)->second.clear();
		effect_bucket_.find(EEffectType::kPBREffect)->second.clear();
		effect_bucket_.find(EEffectType::kShadow)->second.clear();
		for (auto& object : scene_objects_)
		{
			for (auto& effect : object->effects_)
			{
				if (effect == EEffectType::kNone)
				{
					effect_bucket_.find(EEffectType::kNone)->second.push_back(object);
				}	
				else if (effect == EEffectType::kPBREffect)
				{
					effect_bucket_.find(EEffectType::kPBREffect)->second.push_back(object);
				}
				else if (effect == EEffectType::kShadow)
				{
					effect_bucket_.find(EEffectType::kShadow)->second.push_back(object);
				}
			}
		}
	}

}

void Graphics::UpdatePerFrameBuf()
{
	common_v_cbuf_src.gCamreaPos = p_camera_->location_f();
	common_v_cbuf_src.gView = p_camera_->view_matrix();
	common_v_cbuf_src.gProj = p_camera_->projection_matrix();
	common_v_cbuf_src.gViewProj = p_camera_->view_projection_matrix();
	common_p_cbuf_src.gLightInfo = *p_scene_light_;
	CommitPerFrameBuf();
}

ID3D11Device* Graphics::GetDevice()
{
	return pDevice.Get();
}

ID3D11DeviceContext* Graphics::GetContext()
{
	return pDeviceContext.Get();
}

ID3D11DepthStencilView* Graphics::GetDepthStencilView()
{
	return p_depth_stencil_view_.Get();
}

int Graphics::GetWidth() const
{
	return width_;
}

int Graphics::GetHeight() const
{
	return height_;
}

void Graphics::SetRenderTargetView(ID3D11RenderTargetView* target_view)
{
	p_render_targetview_ = target_view;
}

void Graphics::SetDepthStencilView(ID3D11DepthStencilView* depth_view)
{
	p_depth_stencil_view_ = depth_view;
}

//ID3D11ShaderResourceView** Graphics::GetShadowMap()
//{
//	//return g_rtr.GetShaderResourceView();
//}

void Graphics::SetCoordinateType(bool is_world)
{
	p_coordinate_->SetCoordinateType(is_world);
}

bool Graphics::GetCoordinateType() const
{
	return p_coordinate_->GetCoordinateType();
}

HRESULT Graphics::InitializeD3DBase(HWND hWnd, const UINT& w, const UINT& h)
{
	width_ = w;
	height_ = h;
	//create device and context
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0,
		D3D11_CREATE_DEVICE_DEBUG, 0, 0, D3D11_SDK_VERSION, pDevice.GetAddressOf(), &featureLevel, pDeviceContext.GetAddressOf());
	if (FAILED(hr))
	{
		qDebug() << QString("D3D11CreateDevice Failed.");
		return hr;
	}
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		qDebug() << QString("Direct3D FeatureLevel 11 unsupported."); return hr;
	}
	//create swapchain
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = width_;
	sd.BufferDesc.Height = height_;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//多重采样数量和质量级别
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	//将场景渲染到后台缓冲区
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//交换链中的后台缓冲区数量；我们一般只用一个后台缓冲区来实现双缓存。
	sd.BufferCount = 1;
	//将要渲染到的窗口的句柄。
	sd.Windowed = TRUE;
	sd.OutputWindow = hWnd;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	IDXGIFactory* dxgiFactory;
	CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory));
	dxgiFactory->CreateSwapChain(pDevice.Get(), &sd, pSwapChain.GetAddressOf());
	return hr;
}

void Graphics::InitializeSharedBindable()
{
	//创建深度模板缓冲区
	p_depth_stencil_ = std::make_unique<DepthStencil>(static_cast<UINT>(width_), static_cast<UINT>(height_), *this);
	p_depth_stencil_->Bind(*this);
	//创建贴图采样描述
	D3D11_SAMPLER_DESC sample_desc;
	ZeroMemory(&sample_desc, sizeof(sample_desc));
	sample_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sample_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sample_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sample_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sample_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sample_desc.MinLOD = 0;
	sample_desc.MaxLOD = D3D11_FLOAT32_MAX;
	p_sampler_state_ = std::make_unique<SamplerState>(*this, sample_desc);
	p_sampler_state_->Bind(*this);
}

void Graphics::UpdateCameraMovement()
{

	switch (cam_move_state_)
	{
	case ECameraMovementState::kForward:
		p_camera_->AddPosition(p_camera_->forward() * 0.6f);
		break;
	case ECameraMovementState::kBack:
		p_camera_->AddPosition(-p_camera_->forward() * 0.6f);
		break;
	case ECameraMovementState::kRight:
		p_camera_->AddPosition(p_camera_->right() * 0.6f);
		break;
	case ECameraMovementState::kLeft:
		p_camera_->AddPosition(-p_camera_->right() * 0.6f);
		break;
	case ECameraMovementState::kUp:
		p_camera_->AddPosition(p_camera_->up() * 0.6f);
		break;
	case ECameraMovementState::kDown:
		p_camera_->AddPosition(-p_camera_->up() * 0.6f);
		break;
	case ECameraMovementState::kStop:
		break;
	default:
		break;
	}
}

void Graphics::UpdateCameraState(ECameraMovementState new_state)
{
	cam_move_state_ = new_state;
}

ID3D11RenderTargetView* Graphics::pp_render_targetview()
{
	return p_render_targetview_.Get();
}


