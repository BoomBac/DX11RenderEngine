#include "Public\Render\RenderToTexture.h"
#include "ScreenGrab.h"
#include <wincodec.h>
#include "Public\Tool\Utili.h"

#pragma comment(lib,"DirectXTK.lib")

namespace 
{
	ID3D11ShaderResourceView* g_shadow_map;
}


RenderToTexture::RenderToTexture()
{

}

void RenderToTexture::Initialize(Graphics* gfx, ERTTUsage usage)
{
	usage_ = usage;
	switch (usage_)
	{
	case ERTTUsage::kBackBuffer:
	{
		//第一,填充2D纹理形容结构体,并创建2D渲染纹理
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		textureDesc.Width = gfx->GetWidth();
		textureDesc.Height = gfx->GetHeight();
		//textureDesc.Width = 1024;
		//textureDesc.Height = 1024;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;  //纹理像素为12个字节
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		gfx->GetDevice()->CreateTexture2D(&textureDesc, NULL, p_texture_.GetAddressOf());

		//第二，填充渲染目标视图形容体,并进行创建目标渲染视图
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		auto hr = gfx->GetDevice()->CreateRenderTargetView(p_texture_.Get(), &renderTargetViewDesc, p_target_view_.GetAddressOf());

		//第三,填充着色器资源视图形容体,并进行创建着色器资源视图
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		gfx->GetDevice()->CreateShaderResourceView(p_texture_.Get(), &shaderResourceViewDesc, p_shader_view_.GetAddressOf());
	}
		break;
	case ERTTUsage::kDepthBuffer:
	{
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
		depthBufferDesc.Width = gfx->GetWidth();
		depthBufferDesc.Height = gfx->GetHeight();
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;//DXGI_FORMAT_R24G8_TYPELESS
		depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; //24位是为了深度缓存，8位是为了模板缓存
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;  //注意深度缓存(纹理)的绑定标志
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;
		gfx->GetDevice()->CreateTexture2D(&depthBufferDesc, NULL, p_texture_.GetAddressOf());

		//第二,填充深度缓存视图形容结构体,并创建深度缓存视图
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		gfx->GetDevice()->CreateDepthStencilView(p_texture_.Get(), &depthStencilViewDesc, p_depth_view.GetAddressOf());

		//第三,填充着色器资源视图形容体,并进行创建着色器资源视图,注意这是用深度缓存(纹理)来创建的，而不是渲染目标缓存(纹理)创建的
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; //此时因为是仅仅进行深度写，而不是颜色写，所以此时Shader资源格式跟深度缓存是一样的
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = depthBufferDesc.MipLevels;
		gfx->GetDevice()->CreateShaderResourceView(p_texture_.Get(), &shaderResourceViewDesc, p_shader_view_.GetAddressOf());

		//第四，设置视口的属性
		p_view_port.Width = static_cast<float>(gfx->GetWidth());
		p_view_port.Height = static_cast<float>(gfx->GetHeight());
		p_view_port.MinDepth = 0.0f;
		p_view_port.MaxDepth = 1.0f;
		p_view_port.TopLeftX = 0.0f;
		p_view_port.TopLeftY = 0.0f;
	}
		break;
	default:
		break;
	}
}

void RenderToTexture::SetRenderTarget(Graphics* gfx)
{
	if (usage_==ERTTUsage::kBackBuffer)
	gfx->GetContext()->OMSetRenderTargets(1u, p_target_view_.GetAddressOf(), gfx->GetDepthStencilView());
	else if (usage_ == ERTTUsage::kDepthBuffer)
	{
		ID3D11RenderTargetView* renderTarget[1] = { 0 };
		//auto i = gfx->pp_render_targetview();
		gfx->GetContext()->OMSetRenderTargets(1u, renderTarget, p_depth_view.Get());
		//gfx->GetContext()->RSSetViewports(1u, &p_view_port);
	}
}

void RenderToTexture::ClearRenderTarget(Graphics* gfx, float red, float green, float blue, float alpha)
{
	//设置清除缓存为的颜色
	float color[4];
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;
	switch (usage_)
	{
	case ERTTUsage::kBackBuffer:
	{
		//清除背后缓存
		gfx->GetContext()->ClearRenderTargetView(p_target_view_.Get(), color);
		//清除深度缓存和模板缓存
		gfx->GetContext()->ClearDepthStencilView(gfx->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
		break;
	case ERTTUsage::kDepthBuffer:
	{
		gfx->GetContext()->ClearDepthStencilView(p_depth_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	}
		break;
	default:
		break;
	}

}
struct color
{
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE a;
};
void RenderToTexture::SaveToImage(Graphics* gfx,std::string path)
{
	switch (usage_)
	{
	case ERTTUsage::kBackBuffer:
	{
		DirectX::SaveWICTextureToFile(gfx->GetContext(), p_texture_.Get(), GUID_ContainerFormatPng, ToWide(path).c_str());
	}
		break;
	case ERTTUsage::kDepthBuffer:
	{
		auto [pTexTemp, srcTextureDesc] = MakeStaging(gfx);
		const auto width = gfx->GetWidth();
		const auto height = gfx->GetHeight();
		//srcTextureDesc : D3D11_TEXTURE2D_DESC depthBufferDesc;
		//将depthBuffer数据重新解释，并准备填充subResource的buffer
		D3D11_MAPPED_SUBRESOURCE msr = {};
		gfx->GetContext()->Map(pTexTemp.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &msr);
		auto pSrcBytes = static_cast<const char*>(msr.pData);
		bool linearlize = true;
		std::vector<color> textureMap;
		for (unsigned int y = 0; y < height; y++)
		{
			struct Pixel
			{
				char data[4];
			};
			auto pSrcRow = reinterpret_cast<const Pixel*>(pSrcBytes + msr.RowPitch * size_t(y));
			for (unsigned int x = 0; x < width; x++)
			{
				if (srcTextureDesc.Format == DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS)
				{
					const auto raw = 0xFFFFFF & *reinterpret_cast<const unsigned int*>(pSrcRow + x);
					if (linearlize)
					{
						const auto normalized = (float)raw / (float)0xFFFFFF;
						const auto linearized = 0.01f / (1.01f - normalized);
						const auto channel = unsigned char(linearized * 255.0f);
						textureMap.push_back({ channel ,channel ,channel });
					}
					else
					{
						const unsigned char channel = raw >> 16;
						textureMap.push_back({ channel ,channel ,channel });
					}
				}
				else if (srcTextureDesc.Format == DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS)
				{
					const auto raw = *reinterpret_cast<const float*>(pSrcRow + x);
					if (linearlize)
					{
						const auto linearized = 0.01f / (1.01f - raw);
						const auto channel = unsigned char(linearized * 255.0f);
						textureMap.push_back({ channel ,channel ,channel });
					}
					else
					{
						const auto channel = unsigned char(raw * 255.0f);
						textureMap.push_back({ channel ,channel ,channel });
					}
				}
				else
				{
					throw std::runtime_error{ "Bad format in Depth Stencil for conversion to Surface" };
				}
			}
		}
		gfx->GetContext()->Unmap(pTexTemp.Get(), 0u);
		//创建实际要输出的Texture2D
		D3D11_TEXTURE2D_DESC des{};
		des.Height = gfx->GetHeight();
		des.Width = gfx->GetWidth();
		des.CPUAccessFlags = 0;
		des.Usage = D3D11_USAGE_DEFAULT;
		des.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		//des.MipLevels = 0;
		des.MipLevels = 1;
		des.ArraySize = 1;
		des.SampleDesc.Count = 1;
		des.SampleDesc.Quality = 0;
		ID3D11Texture2D* surface;
		D3D11_SUBRESOURCE_DATA sd;
		color* pdata = textureMap.data();
		sd.pSysMem = pdata;
		sd.SysMemPitch = width * sizeof(color);
		gfx->GetDevice()->CreateTexture2D(&des, &sd, &surface);
		DirectX::SaveWICTextureToFile(gfx->GetContext(), surface, GUID_ContainerFormatPng, ToWide(path).c_str());

		//{
		//	D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
		//	srvd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		//	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		//	srvd.Texture2D.MostDetailedMip = 0;
		//	srvd.Texture2D.MipLevels = des.MipLevels;
		//	gfx->GetDevice()->CreateShaderResourceView(p_texture_.Get(), &srvd, p_shader_view_.GetAddressOf());
		//	g_shadow_map = surface;
		//}
	}
		break;
	default:
		break;
	}
}

void RenderToTexture::SaveToImage(Graphics* gfx)
{
	switch (usage_)
	{
	case ERTTUsage::kBackBuffer:
		SaveToImage(gfx, "Y:/Project_VS2019/DX11RenderEngine/Debug/Back.png");
		break;
	case ERTTUsage::kDepthBuffer:
		SaveToImage(gfx, "Y:/Project_VS2019/DX11RenderEngine/Debug/Depth.png");
		break;
	default:
		break;
	}
}

std::pair<Microsoft::WRL::ComPtr<ID3D11Texture2D>, D3D11_TEXTURE2D_DESC> RenderToTexture::MakeStaging(Graphics* gfx)
{
	// get info about the stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC srcViewDesc{};
	p_depth_view->GetDesc(&srcViewDesc);
	// creating a temp texture compatible with the source, but with CPU read access
	Microsoft::WRL::ComPtr<ID3D11Resource> pResSource;
	p_depth_view->GetResource(&pResSource);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexSource;
	pResSource.As(&pTexSource);
	D3D11_TEXTURE2D_DESC srcTextureDesc{};
	pTexSource->GetDesc(&srcTextureDesc);
	D3D11_TEXTURE2D_DESC tmpTextureDesc = srcTextureDesc;
	tmpTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	tmpTextureDesc.Usage = D3D11_USAGE_STAGING;
	tmpTextureDesc.BindFlags = 0;
	tmpTextureDesc.MiscFlags = 0;
	tmpTextureDesc.ArraySize = 1;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexTemp;
	gfx->GetDevice()->CreateTexture2D(&tmpTextureDesc, nullptr, &pTexTemp);

	// copy texture contents
	if (srcViewDesc.ViewDimension == D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2DARRAY)
	{
		// source is actually inside a cubemap texture, use view info to find the correct slice and copy subresource
		gfx->GetContext()->CopySubresourceRegion(pTexTemp.Get(), 0, 0, 0, 0, pTexSource.Get(), srcViewDesc.Texture2DArray.FirstArraySlice, nullptr);
	}
	else
	{
		gfx->GetContext()->CopyResource(pTexTemp.Get(), pTexSource.Get());
	}
	return { std::move(pTexTemp),srcTextureDesc };
}

ID3D11ShaderResourceView** RenderToTexture::GetShaderResourceView()
{
	return p_shader_view_.GetAddressOf();
}
