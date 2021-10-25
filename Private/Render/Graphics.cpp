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




#pragma comment(lib,"d3d11.lib") 
#pragma comment(lib,"dxgi.lib") 
#pragma comment(lib,"D3DCompiler.lib")



template<typename T>
using Vec = std::vector<T, std::allocator<T>>;

Drawable* Graphics::p_selected_object_ = nullptr;
Drawable* Graphics::p_coordinate_ = nullptr;




namespace 
{
	//RenderToTexture g_rtr;
}

Graphics::Graphics(HWND hWnd)
{
	InitializeD3DBase(hWnd,800,600);

	ResizeBackbuffer(800, 600);
	InitializeSharedBindable();

	static float color[] = { 0,0,0,1 };
	bg_color = color;
	shadowParma.light_far = 1000.f;
	shadowParma.light_near = 1.f;
	shadowParma.light_size = 0.4f;
	shadowParma.max_bias = 0.001f;
	p_shadow_effect_ = &shadowParma;
	materal_property_.albedo = { 1.f,0.86f,0.76f };
	materal_property_.metallic = 0.f;
	materal_property_.roughness = 0.1f;
	p_material_property_ = &materal_property_;

	p_camera_ = new Camera();
	p_camera_->SetProjection(75.f, 4.f / 3.f, 1.f, 1000.f);
	camera_set_.push_back(p_camera_);

	cam_move_state_ = ECameraMovementState::kStop;
	outline_notify_ = new Subject();
	//初始化工厂类
	ResManage::InitResManage(this);
	GeometryFactory geo_factory(this);
	TextureFactory::GetInstance().AddTexture("Y:/Project_VS2019/DX11RenderEngine/Res/Texture/height.jpg");
	TextureFactory::GetInstance().AddTexture("Y:/Project_VS2019/DX11RenderEngine/Debug/Depth.png");
	TextureFactory::GetInstance().AddTexture("Y:/Project_VS2019/DX11RenderEngine/Res/Texture/rustediron2_basecolor.png");
	TextureFactory::GetInstance().AddTexture("Y:/Project_VS2019/DX11RenderEngine/Res/Texture/rustediron2_metallic.png");
	TextureFactory::GetInstance().AddTexture("Y:/Project_VS2019/DX11RenderEngine/Res/Texture/rustediron2_normal.png");
	TextureFactory::GetInstance().AddTexture("Y:/Project_VS2019/DX11RenderEngine/Res/Texture/rustediron2_roughness.png");


	//LoadResource();
	////初始化场景默认灯光
	//default_light_.light_color_ = { 1.f,1.f,1.f,1.f };
	//default_light_.light_dir_ = { 0.f,-1.f,0.f };
	//default_light_.light_intensity_ = 0.2f;
	//default_light_.light_type = 1.f;
	//p_scene_light_ = &default_light_;

	default_light_shader_.light_color_ = { 1.f,1.f,1.f };
	default_light_shader_.light_intensity_ = 0.2f;
	p_light_shader_ = &default_light_shader_;


	//g_light_camera.SetProjection(75.f, 4.f / 3.f, 0.0001f, 10000.f);
	//g_light_camera.SetLocation({ 0.f, 10.f, 50.f,0.f });
	//g_light_camera.SetRotation(0.f, DegToRad(180.f), 0.f);
	//p_light_camera = &g_light_camera;


	MeshFactory::getInstance().AddMesh("Y:/Project_VS2019/DX11RenderEngine/Res/Mesh/point_light.obj");
	MeshFactory::getInstance().AddMesh("Y:/Project_VS2019/DX11RenderEngine/Res/Mesh/directional_light.obj");
	MeshFactory::getInstance().AddMesh("Y:/Project_VS2019/DX11RenderEngine/Res/Mesh/spot_light.obj");

	//创建坐标轴
	p_coordinate_ = new Coordinate(*this, 10.f);
	scene_objects_.push_back(dynamic_cast<Drawable*>(p_coordinate_));

	AddLight(ELightType::kPonintLight);
	//创建默认灯光
	p_light_->SetWorldLocation({ 20.f,30.f,0.f });
	dynamic_cast<PointLight*>(p_light_)->SetRadius(100.f);
	p_light_camera = &dynamic_cast<Light*>(p_light_)->light_camera_;
	p_light_matrix_ = dynamic_cast<Light*>(p_light_)->GetLightMatrix();


	MeshFactory::getInstance().AddMesh("Y:/Project_VS2019/DX11RenderEngine/Res/Mesh/sphere.obj");
	MeshFactory::getInstance().AddMesh("Y:/Project_VS2019/DX11RenderEngine/Res/Mesh/plane.obj");

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
	dynamic_cast<Light*>(p_light_)->UpdateLightMatrix();
	pDeviceContext->ClearRenderTargetView(p_render_targetview_.Get(), bg_color);
	//p_sky_box_->GenerateCubeSurface(1024,EGenerateFlag::kSpecular);
	//g_rtr.ClearRenderTarget(this, 0.f, 1.f, 1.f, 1.f);
	////isRenderShaodw = true;
	////// Pender To Texture
	//g_rtr.SetRenderTarget(this);
	//p_sky_box_->Draw(*this);
	//g_rtr.SaveToImage(this, "C:/Users/BoomBac/Desktop/test.png");
	//for (const auto& i : scene_objects_)
	//{
	//	i->Draw(*this);
	//}

	//GetContext()->OMSetRenderTargets(1, p_render_targetview_.GetAddressOf(), nullptr);
	//Render Scene

	p_depth_stencil_->Clear(*this);
	GetContext()->OMSetRenderTargets(1, p_render_targetview_.GetAddressOf(), p_depth_stencil_->pDepthStencilView.Get());
	isRenderShaodw = false;
	for (const auto& i : scene_objects_)
	{
		i->Draw(*this);
	}
	p_sky_box_->Draw(*this);
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

void Graphics::SetSelectedObjectTranslate(const CusMath::vector3d& t)
{
	if (dynamic_cast<Coordinate*>(p_coordinate_)->GetCoordinateType())
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
	if (dynamic_cast<Coordinate*>(p_coordinate_)->GetCoordinateType())
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
	outline_notify_->NotifyObserver(true);
}


void Graphics::DeleteSceneObject(int index)
{
	auto it = scene_objects_.begin();
	it += index;
	delete *it;
	scene_objects_.erase(it);
}

void Graphics::InitSceneObject()
{
	GeometryFactory::GenerateGeometry("sphere.obj");
	auto plane = GeometryFactory::GenerateGeometry("plane.obj");
	plane->SetActorScale({ 5.f,1.f,5.f });
	p_sky_box_ = std::make_unique<SkyBox>(*this);
	//set skybox as environment_map at slot 6
	//auto p = p_sky_box_->GetEnvironmentRsv();
	//pDeviceContext->PSSetShaderResources(6, 1, &p);
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
			p_light_ = p_selected_object_;
			//p_camera_ = &dynamic_cast<Light*>(p_light_)->light_camera_;
			p_scene_light_ = dynamic_cast<Light*>(p_selected_object_)->GetAttritute();
			p_light_camera = &dynamic_cast<Light*>(p_light_)->light_camera_;
			p_light_matrix_ = dynamic_cast<Light*>(p_light_)->GetLightMatrix();
			//p_light_view_projection_ = &dynamic_cast<Light*>(p_light_)->GetLightMatrix()->view_porjection;
			//p_light_view_projection_ = dynamic_cast<Light*>(p_light_)->light_camera_.view_projection_matrix();
		}
		//else
		//{
		//	p_camera_ = camera_set_[0];
		//}
	}
	else
	SetSelectObject(nullptr);
}
void Graphics::ResizeBackbuffer(const UINT& w, const UINT& h)
{
	static bool isInitialize = false;
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
		//p_depth_stencil_view_->Release();
		//pDeviceContext->Flush();
		//pDeviceContext->ClearState();
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
	}

}
void Graphics::SetSelectObject(Drawable* object)
{
	p_selected_object_ = object;
	dynamic_cast<Coordinate*>(p_coordinate_)->SetAttachedObject(p_selected_object_);
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
	dynamic_cast<Coordinate*>(p_coordinate_)->SetCoordinateType(is_world);
	//if(is_world)
	//ResizeBackbuffer(1920, 1080);
	//else
	//{
	//	ResizeBackbuffer(800, 600);
	//}
}

bool Graphics::GetCoordinateType() const
{
	return dynamic_cast<Coordinate*>(p_coordinate_)->GetCoordinateType();
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


