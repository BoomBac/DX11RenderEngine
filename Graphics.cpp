#include "Graphics.h"
#include <d3d11.h>
#include <QDebug>
#include "vector2D.h"
#include <d3dcompiler.h>
#include <vector>
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "InputLayout.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "VertexLayout.h"

#pragma comment(lib,"d3d11.lib") 
#pragma comment(lib,"dxgi.lib") 
#pragma comment(lib,"D3DCompiler.lib")

template<typename T>
using Vec = std::vector<T, std::allocator<T>>;


struct ConstantBuffer
{
	struct 
	{
		float elem[4][4];
	}transformation;
};
//const ConstantBuffer cb =
//{
//	//行优先矩阵，行主序，可以直接传入hlsl的mul，无需转置
//	{
//		0.75f*std::cos(45.f),std::sin(45.f),0.f,0.f,
//		0.75f * -std::sin(45.f),std::cos(45.f),0.f,0.f,
//		0.f,0.f,1.f,0.f,
//		0.f,0.f,0.f,1.f,
//	}
//};



Graphics::Graphics(HWND hWnd)
{
	InitDx11(hWnd);
	static float color[] = { 1,0,0,1 };
	bg_color = color;
}

Graphics::~Graphics()
{
}

void Graphics::EndFrame()
{
	DrawTestGraph();
	pDeviceContext->ClearRenderTargetView(pRenderTargetView.Get(), bg_color);
	//pDeviceContext->Draw(3u, 0u);
	pDeviceContext->DrawIndexed(6u, 0u,0u);
	pSwapChain->Present(0u, 0u);
	
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
	return hr;
}

void Graphics::DrawTestGraph()
{
//------------------------------------------------输入装配阶段IA
	////输入顶点数据
	//const CusMath::vector2d vertices[] = {

	//	/*
	//		.1    .2
	//		.3    .4
	//	*/
	//};
	std::vector<CusMath::vector2d> vertices{
			{-0.5,0.5},
		{0.5,0.5},
		{0.5,-0.5},
		{-0.5,-0.5}
	};
	VertexBuffer<CusMath::vector2d, Vec> vb(vertices,pDevice.Get());
	vb.tBind(*(pDeviceContext.Get()));
	////创建顶点缓冲区
	//D3D11_BUFFER_DESC bd = {};
	//ZeroMemory(&bd, sizeof(bd));
	//bd.Usage = D3D11_USAGE_DEFAULT;
	//bd.ByteWidth = sizeof(vertices);
	//bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//bd.CPUAccessFlags = 0;
	//D3D11_SUBRESOURCE_DATA InitData;
	//ZeroMemory(&InitData, sizeof(InitData));
	//InitData.pSysMem = &vertices.at(0);
	//Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	//pDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer);



	//创建索引缓冲区

	//const unsigned short indices[] =
	//{
	//	0,1,2,
	//	0,2,3
	//};
	std::vector<unsigned short> indices{
		0,1,2,
		0,2,3
	};
	IndexBuffer<unsigned short, Vec> ib(indices,pDevice.Get());
	ib.tBind(*pDeviceContext.Get());
	//D3D11_BUFFER_DESC Ibd = {};
	//Ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//Ibd.Usage = D3D11_USAGE_DEFAULT;
	//Ibd.CPUAccessFlags = 0;
	//Ibd.MiscFlags = 0;
	//Ibd.ByteWidth = sizeof(indices);
	//Ibd.StructureByteStride = sizeof(unsigned short);
	//Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	//D3D11_SUBRESOURCE_DATA isd = {};
	//isd.pSysMem = indices;
	//pDevice->CreateBuffer(&Ibd, &isd, &pIndexBuffer);



	PixelShader ps(pDevice.Get(), "PixelShader.cso");
	ps.TBind(*pDeviceContext.Get());

	//Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob = nullptr;
	// 编译创建像素着色器
	//Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	//D3DReadFileToBlob(L"PixelShader.cso", &pVSBlob);
	//pDevice->CreatePixelShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pPixelShader);
	//pDeviceContext->PSSetShader(pPixelShader.Get(), nullptr, 0);

	//pDeviceContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);
	VertexShader vs(pDevice.Get(), "VertexShader.cso");
	vs.TBind(*pDeviceContext.Get());
	// 编译创建顶点着色器
	//Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	//D3DReadFileToBlob(L"VertexShader.cso", &pVSBlob);
	//pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr,
	//	pVertexShader.GetAddressOf());

	const ConstantBuffer cb =
	{
		//行优先矩阵，行主序，可以直接传入hlsl的mul，无需转置
		{
			0.75f * std::cos(45.f),std::sin(45.f),0.f,0.f,
			0.75f * -std::sin(45.f),std::cos(45.f),0.f,0.f,
			0.f,0.f,1.f,0.f,
			0.f,0.f,0.f,1.f,
		}
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cdb;
	cdb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cdb.Usage = D3D11_USAGE_DYNAMIC;
	cdb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cdb.MiscFlags = 0;
	cdb.ByteWidth = sizeof(cb);
	cdb.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	pDevice->CreateBuffer(&cdb, &csd, pConstantBuffer.GetAddressOf());

	pDeviceContext->VSSetConstantBuffers(0, 1, pConstantBuffer.GetAddressOf());

	VertexLayout vl;
	vl << VertexType::Position2D;
	vl.Build();
	InputLayout il(pDevice.Get(),vs,vl);
	il.TBind(*pDeviceContext.Get());

	//定义输入布局描述
	//Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	//const D3D11_INPUT_ELEMENT_DESC layout[] =
	//{
	//	{"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	//	//{"Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
	//};
	//UINT numElements = ARRAYSIZE(layout);

	////创建输入布局
	//pDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
	//	pVSBlob->GetBufferSize(), pInputLayout.GetAddressOf());



	//绑定
	//绑定顶点缓冲
	//UINT stride = sizeof(CusMath::vector2d);
	//UINT offset = 0u;
	//pDeviceContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
	//pDeviceContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	//pDeviceContext->IASetInputLayout(pInputLayout.Get());

	// 设置图元拓扑
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//pDeviceContext->VSSetShader(pVertexShader.Get(), nullptr, 0);

	pDeviceContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);

}
