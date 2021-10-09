#include "Public/Render/Light/Light.h"



Light::Light(Graphics& gfx, const char* light_type) : Model::Model(gfx, light_type)
{
	attritute_.light_intensity_ = 0.8f;
	attritute_.light_color_ = {1.f,1.f,1.f,1.f};
}

LightSet Light::GetLightAttribute()
{
	return attritute_;
}

void Light::SetColor(DirectX::XMFLOAT4 color)
{
	attritute_.light_color_ = color;
}

void Light::SetIntensity(float intensity)
{
	attritute_.light_intensity_ = intensity;
}


