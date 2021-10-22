#include "Public/Render/Bindable/RasterizerState.h"

RasterizerState::RasterizerState(Graphics& gfx, const D3D11_RASTERIZER_DESC& des)
{
	gfx.GetDevice()->CreateRasterizerState(&des, p_raster_state_.GetAddressOf());
}

RasterizerState::RasterizerState(Graphics& gfx)
{
	p_raster_state_ = nullptr;
}

void RasterizerState::Bind(Graphics& gfx)
{
	gfx.GetContext()->RSSetState(p_raster_state_.Get());
}

EBindableType RasterizerState::GetType() const
{
	return EBindableType::kRasterizerState;
}
