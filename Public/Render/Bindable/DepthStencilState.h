#ifndef DX11ENGINE_RENDER_BINDABLE_DEPTHSTENCILSTATE_H
#define DX11ENGINE_RENDER_BINDABLE_DEPTHSTENCILSTATE_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

#include "BindableInterface.h"

class DepthStencilState :
	public BindableInterface
{
public:
	DepthStencilState(Graphics& gfx,const D3D11_DEPTH_STENCIL_DESC& des);
	virtual void Bind(Graphics& gfx) override;
	virtual EBindableType GetType() const override;
private:
	DISALLOW_COPY_AND_ASSIGN(DepthStencilState)
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> p_ds_state_;
};

#endif //DX11ENGINE_RENDER_BINDABLE_DEPTHSTENCILSTATE_H
