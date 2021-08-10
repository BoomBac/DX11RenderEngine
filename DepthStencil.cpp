#include "DepthStencil.h"
#include <QDebug>

DepthStencil::DepthStencil(UINT x, UINT y, Graphics& gfx):w(x),h(y)
{
	qDebug() << "DepthStencil construct";
	D3D11_TEXTURE2D_DESC dsd;
	dsd.Width = x;		//与后台尺寸一致
	dsd.Height = y;
	dsd.MipLevels = 1;
	dsd.ArraySize = 1;
	dsd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsd.SampleDesc.Count = 4;	//与交换链一致
	dsd.SampleDesc.Quality = 0.2;
	dsd.Usage = D3D11_USAGE_DEFAULT;
	dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsd.CPUAccessFlags = 0;
	dsd.MiscFlags = 0;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
	GetDevice(gfx)->CreateTexture2D(&dsd, 0, &pDepthStencil);
	GetDevice(gfx)->CreateDepthStencilView(pDepthStencil.Get(), 0, &pDepthStencilView);
	//绑定缓冲至渲染管线，在没有设置DepthStenciState之前，会采用默认状态，默认开启深度测试
}

void DepthStencil::Bind(Graphics& gfx)
{
	//GetContext(gfx)->OMSetRenderTargets(1, &mRenderTargetView, pDepthStencilView.Get());
}

void DepthStencil::TBind(Graphics& gfx, ID3D11RenderTargetView* rTV)
{
	GetContext(gfx)->OMSetRenderTargets(1, &rTV, pDepthStencilView.Get());
}

EBindableType DepthStencil::GetType() const
{
	return EBindableType::DepthStencilBuffer;
}

void DepthStencil::Clear(Graphics& gfx)
{
	GetContext(gfx)->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
}
