#include "Public/Render/Light/Light.h"



Light::Light(Graphics& gfx, const char* light_type) : Model::Model(gfx, light_type)
{
	attritute_.light_intensity_ = 0.2f;
	attritute_.light_color_ = {1.f,1.f,1.f,1.f};
	raw_shader_.light_intensity_ = 0.2f;
	raw_shader_.light_color_ = {1.f,1.f,1.f};
	gfx.p_scene_light_ = &attritute_;
	gfx.p_light_shader_ = &raw_shader_;
}

void Light::UpdateAttribute()
{

}

void Light::SetColor(DirectX::XMFLOAT4 color)
{
	attritute_.light_color_ = color;
	raw_shader_.light_color_.x = color.x;
	raw_shader_.light_color_.y = color.y;
	raw_shader_.light_color_.z = color.z;
}

void Light::SetIntensity(float intensity)
{
	attritute_.light_intensity_ = intensity;
	raw_shader_.light_intensity_ = intensity;
}

void Light::Draw(Graphics& gfx)
{
	UpdateAttribute();
	Model::Draw(gfx);
}

ELightType Light::GetLightType() const
{
	return light_type_;
}

LightSet* Light::GetAttritute()
{
	return &attritute_;
}

