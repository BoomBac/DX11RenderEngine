#pragma once

#include "Bindable.h"
class DepthStencil :
 public Bindable
{
public:
	DepthStencil(UINT x,UINT y, Graphics& gfx);
	virtual void Bind(Graphics& gfx) override;
	void TBind(Graphics& gfx,ID3D11RenderTargetView* rTV);
	virtual EBindableType GetType() const override;
	void Clear(Graphics& gfx);

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
private:

	UINT w;
	UINT h;
};

