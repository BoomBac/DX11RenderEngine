#include "Public/Render/Graphics.h"
#include <d3d11.h>
#include "vector2D.h"
#include <d3dcompiler.h>
#include "Public/Global.h"
#include "Public/Render/Bindable/DepthStencil.h"
#include "Public/Render/Shape/Box.h"

#include "Public/Tool/Utili.h"
#include "Public/Render/Drawable/Coordinate.h"
#include "Public/Render/GeometryFactory.h"
#include <utility>
#include "Public/Render/ModelResFactory.h"

#pragma comment(lib,"d3d11.lib") 
#pragma comment(lib,"dxgi.lib") 
#pragma comment(lib,"D3DCompiler.lib")

template<typename T>
using Vec = std::vector<T, std::allocator<T>>;

Drawable* Graphics::p_selected_object_ = nullptr;
Drawable* Graphics::p_coordinate_ = nullptr;


namespace
{

}

Graphics::Graphics(HWND hWnd)
{
	InitDx11(hWnd);
	static float color[] = { 0,0,0,1 };
	bg_color = color;
	camera_.SetProjection(75.f,4.f/3.f,0.1,1000.f);
	cam_move_state_ = ECameraMovementState::kStop;
	outline_notify_ = new Subject();
	//初始化工厂类
	GeometryFactory geo_factory(this);
	ModelResFactory mres_factory(this);
	mres_factory.AddResource(0);
	//初始化坐标轴和场景物体
	InitSceneObject();
}



Graphics::~Graphics()
{
	if (dsbuffer != nullptr)
		delete dsbuffer;
	if (outline_notify_ != nullptr)
		delete outline_notify_;
	if (!scene_objects_.empty())
	{
		for (auto i : scene_objects_)
		{
			delete i;
		}
	}
}

void Graphics::EndFrame()
{
	pDeviceContext->ClearRenderTargetView(p_render_targetview_.Get(), bg_color);
	dsbuffer->Clear(*this);
	UpdateCameraMovement();
	for (const auto& i : scene_objects_)
	{
		i->Draw(*this);
	}
	pSwapChain->Present(0u, 0u);
}

void Graphics::DrawIndexed(const UINT& count)
{
	pDeviceContext->DrawIndexed(count, 0u, 0u);
	//pDeviceContext->Draw(26u,0u);
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
	//创建坐标轴
	//p_coordinate_ = new Coordinate(*this, 10.f);
	//scene_objects_.push_back(dynamic_cast<Drawable*>(p_coordinate_));
	GeometryFactory::GenerateGeometry(EGeometryType::kCustom);
	//GeometryFactory::GenerateGeometry(EGeometryType::kCustom);
	//GeometryFactory::GenerateGeometry(EGeometryType::kBox);
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

void Graphics::SetSelectObject(const int& index)
{
	if (index!=-1)
	SetSelectObject(scene_objects_[index]);
	else
	SetSelectObject(nullptr);
}
void Graphics::SetSelectObject(Drawable* object)
{
	p_selected_object_ = object;
	dynamic_cast<Coordinate*>(p_coordinate_)->SetAttachedObject(p_selected_object_);
}

void Graphics::SetCoordinateType(bool is_world)
{
	dynamic_cast<Coordinate*>(p_coordinate_)->SetCoordinateType(is_world);
}

bool Graphics::GetCoordinateType() const
{
	return dynamic_cast<Coordinate*>(p_coordinate_)->GetCoordinateType();
}

HRESULT Graphics::InitDx11(HWND hWnd)
{
	//创建设备和上下文
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
	//创建交换链
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 800;
	sd.BufferDesc.Height = 600;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	//多重采样数量和质量级别
	sd.SampleDesc.Count = 4;
	sd.SampleDesc.Quality = 0.2;

	//将场景渲染到后台缓冲区
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//交换链中的后台缓冲区数量；我们一般只用一个后台缓冲区来实现双缓存。
	sd.BufferCount = 1;
	//将要渲染到的窗口的句柄。
	sd.Windowed = TRUE;
	sd.OutputWindow = hWnd;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	IDXGIFactory* dxgiFactory;
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&dxgiFactory));
	dxgiFactory->CreateSwapChain(pDevice.Get(), &sd, pSwapChain.GetAddressOf());
	if (FAILED(hr)) return hr;

	ID3D11Texture2D* backBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if(backBuffer!=nullptr)
		hr = pDevice->CreateRenderTargetView(backBuffer, NULL, p_render_targetview_.GetAddressOf());
	else
	{
		qDebug() << "backBuffer is null!";
	}

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	pDeviceContext->RSSetViewports(1, &vp);
	//创建深度模板缓冲区
	dsbuffer = new DepthStencil(800u, 600u, *this);
	dsbuffer->Bind(*this);
	return hr;
}

void Graphics::UpdateCameraMovement()
{
	switch (cam_move_state_)
	{
	case ECameraMovementState::kForward:
		camera_.AddPosition(camera_.forward() * 0.6f);
		break;
	case ECameraMovementState::kBack:
		camera_.AddPosition(-camera_.forward() * 0.6f);
		break;
	case ECameraMovementState::kRight:
		camera_.AddPosition(camera_.right() * 0.6f);
		break;
	case ECameraMovementState::kLeft:
		camera_.AddPosition(-camera_.right() * 0.6f);
		break;
	case ECameraMovementState::kUp:
		camera_.AddPosition(camera_.up() * 0.6f);
		break;
	case ECameraMovementState::kDown:
		camera_.AddPosition(-camera_.up() * 0.6f);
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


