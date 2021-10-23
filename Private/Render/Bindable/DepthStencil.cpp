#include "Public\Render\Bindable\DepthStencil.h"


DepthStencil::DepthStencil(const UINT& x, const UINT& y, Graphics& gfx):w_(x),h_(y)
{
	D3D11_TEXTURE2D_DESC dsd;
	dsd.Width = x;		//与后台尺寸一致
	dsd.Height = y;
	dsd.MipLevels = 1;
	dsd.ArraySize = 1;
	dsd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsd.SampleDesc.Count = 1;	//与交换链一致
	dsd.SampleDesc.Quality = 0;
	dsd.Usage = D3D11_USAGE_DEFAULT;
	dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsd.CPUAccessFlags = 0;
	dsd.MiscFlags = 0;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
	GetDevice(gfx)->CreateTexture2D(&dsd, 0, &pDepthStencil);
	GetDevice(gfx)->CreateDepthStencilView(pDepthStencil.Get(), 0, &pDepthStencilView);
	//绑定缓冲至渲染管线，在没有设置DepthStenciState之前，会采用默认状态，默认开启深度测试
	gfx.SetDepthStencilView(pDepthStencilView.Get());
}

void DepthStencil::Bind(Graphics& gfx)
{
	auto adress = gfx.pp_render_targetview();
	GetContext(gfx)->OMSetRenderTargets(1, &adress, pDepthStencilView.Get());
}

EBindableType DepthStencil::GetType() const
{
	return EBindableType::kDepthStencilBuffer;
}

void DepthStencil::Clear(Graphics& gfx)
{
	GetContext(gfx)->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
}
