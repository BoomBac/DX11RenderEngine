#include "Public/Render/Light/DirectionalLight.h"

DirectionalLight::DirectionalLight(Graphics& gfx) : Light::Light(gfx, "directional_light.obj")
{
	attritute_.light_type = 1.f;
	direction_ = { 0.f,-1.f,0.f };
	light_type_ = ELightType::kDirectionLight;
}

void DirectionalLight::UpdateAttribute()
{
	direction_ = { 0.f,-1.f,0.f };
	DirectX::XMStoreFloat3(&direction_, DirectX::XMVector3TransformNormal(XMLoadFloat3(&direction_), GetRotationMartix()));
	attritute_.light_dir_ = direction_;
}

