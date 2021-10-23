#ifndef DX11ENGINE_RENDER_BINDABLE_SAMPLERSTATE_H
#define DX11ENGINE_RENDER_BINDABLE_SAMPLERSTATE_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif


#include "BindableInterface.h"

class SamplerState :
	public BindableInterface
{
public:
	SamplerState(Graphics& gfx,const D3D11_SAMPLER_DESC& des);
	virtual void Bind(Graphics& gfx) override;
	virtual EBindableType GetType() const override;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> p_sample_state_;
private:
	DISALLOW_COPY_AND_ASSIGN(SamplerState)
};







#endif //DX11ENGINE_RENDER_BINDABLE_SAMPLERSTATE_H