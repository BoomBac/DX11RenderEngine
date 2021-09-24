#ifndef DX11ENGINE_RENDER_BINDABLE_DEPTHSTENCIL_H
#define DX11ENGINE_RENDER_BINDABLE_DEPTHSTENCIL_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

#include "BindableInterface.h"

class DepthStencil :
 public BindableInterface
{
public:
	DepthStencil(UINT x,UINT y, Graphics& gfx);
	virtual void Bind(Graphics& gfx) override;
	virtual EBindableType GetType() const override;
	void Clear(Graphics& gfx);

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
private:
	DISALLOW_COPY_AND_ASSIGN(DepthStencil)
	UINT w_;
	UINT h_;
};

#endif //DX11ENGINE_RENDER_BINDABLE_DEPTHSTENCIL_H