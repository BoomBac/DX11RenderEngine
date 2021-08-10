#include "Graphics.h"
#include <d3d11.h>
#include <QDebug>
#include "vector2D.h"
#include <d3dcompiler.h>
#include "Global.h"
#include <QDebug>
#include "DepthStencil.h"
#include "Box.h"
#include "Drawable.h"

#pragma comment(lib,"d3d11.lib") 
#pragma comment(lib,"dxgi.lib") 
#pragma comment(lib,"D3DCompiler.lib")

template<typename T>
using Vec = std::vector<T, std::allocator<T>>;

//当前渲染视口选中的物体
static Drawable* SelectedObject = nullptr;

Graphics::Graphics(HWND hWnd)
{
	InitDx11(hWnd);
	static float color[] = { 0,0,0,1 };
	bg_color = color;
	// CusMath::vector3d(0.f, 0.f, 0.f),5.f,*this);//  Box(CusMath::vector3d(0.f, 0.f, 0.f), 5, *this);
	SceneObjects.push_back(dynamic_cast<Drawable*>(new Box(CusMath::vector3d(0.f, 0.f, 0.f), 2, *this)));
	SceneObjects.push_back(dynamic_cast<Drawable*>(new Box(CusMath::vector3d(0.f, 0.f, 0.f), 3, *this)));
	SelectedObject = SceneObjects[0];

}

Graphics::~Graphics()
{
	if (dsbuffer != nullptr)
		delete dsbuffer;
	if (!SceneObjects.empty())
	{
		for (auto i : SceneObjects)
		{
			delete i;
		}
	}
}

void Graphics::EndFrame()
{
	pDeviceContext->ClearRenderTargetView(pRenderTargetView.Get(), bg_color);
	dsbuffer->Clear(*this);
	for (auto i : SceneObjects)
	{
		SceneObjects[0]->Draw(*this);
	}
	SceneObjects[1]->SetActorLocation({ 10.f,0.f,0.f });
	//box1->Update(DirectX::XMMatrixTranslation(0.f, 0.f, 0.f));
	pSwapChain->Present(0u, 0u);
	
}

void Graphics::DrawIndexed(const UINT& count)
{
	pDeviceContext->DrawIndexed(count, 0u, 0u);
}

#define TORAD(x) x*DirectX::XM_PI/180.f
void Graphics::SetCameraTransformation(const float& x, const float& y, const float& z)
{
	static float preAngle[3] = { 0.f,0.f,0.f };	
	Drawable::UpdateCameraTransformation(
		DirectX::XMMatrixRotationRollPitchYaw(TORAD((x - preAngle[0])),
			TORAD((y - preAngle[1])), TORAD((z - preAngle[2]))
				));
	preAngle[0] = x;
	preAngle[1] = y;
	preAngle[2] = z;
	SelectedObject->OnCameraTransChanged();
}
#undef TORAD
void Graphics::SetVPBackColor(float color[4])
{
	bg_color = color;
}



void Graphics::SetSelectedObjectTranslate(const CusMath::vector3d& t)
{
	SelectedObject->SetActorLocation(t);
}

void Graphics::SetSelectedObjectRotation(const CusMath::vector3d& t)
{
	SelectedObject->SetActorRotation(t);
}

void Graphics::SetSelectedObjectScale(const CusMath::vector3d& t)
{
	SelectedObject->SetActorScale(t);
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
		hr = pDevice->CreateRenderTargetView(backBuffer, NULL, pRenderTargetView.GetAddressOf());
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
	//dsbuffer->TBind(*this, pRenderTargetView.Get());

	pDeviceContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);
	return hr;
}


