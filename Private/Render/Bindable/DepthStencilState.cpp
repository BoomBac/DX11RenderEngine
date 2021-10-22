#include "Public/Render//Bindable/DepthStencilState.h"

DepthStencilState::DepthStencilState(Graphics& gfx, const D3D11_DEPTH_STENCIL_DESC& des)
{
	gfx.GetDevice()->CreateDepthStencilState(&des, p_ds_state_.GetAddressOf());
}

void DepthStencilState::Bind(Graphics& gfx)
{
	gfx.GetContext()->OMSetDepthStencilState(p_ds_state_.Get(),1u);
}

EBindableType DepthStencilState::GetType() const
{
	return EBindableType::kDepthStencilState;
}
