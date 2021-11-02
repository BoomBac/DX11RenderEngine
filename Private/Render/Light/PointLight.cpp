#include "Public/Render/Light/PointLight.h"

PointLight::PointLight(Graphics& gfx) : Light::Light(gfx,"point_light.obj")
{
	attritute_.light_type = 0.f;
	attritute_.affect_radius_ = 20.f;
	SetWorldLocation({ 0.f,50.f,50.f });
	attritute_.light_pos_ = light_camera_.location_f();
	light_type_ = ELightType::kPonintLight;
}

void PointLight::UpdateAttribute()
{
	Light::UpdateAttribute();
	attritute_.light_pos_ = light_camera_.location_f();
	SetWorldLocation({ attritute_.light_pos_.x,attritute_.light_pos_.y,attritute_.light_pos_.z });
}

void PointLight::SetRadius(float radius)
{
	attritute_.affect_radius_ = radius;
}
