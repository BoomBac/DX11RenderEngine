#include "Public/Render/Bindable/SamplerState.h"

SamplerState::SamplerState(Graphics& gfx, const D3D11_SAMPLER_DESC& des)
{
	gfx.GetDevice()->CreateSamplerState(&des, p_sample_state_.GetAddressOf());

}

void SamplerState::Bind(Graphics& gfx)
{
	gfx.GetContext()->PSSetSamplers(0u, 1u, p_sample_state_.GetAddressOf());
}

EBindableType SamplerState::GetType() const
{
	return EBindableType::kSamplerState;
}
