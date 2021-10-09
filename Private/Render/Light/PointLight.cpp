#include "Public/Render/Light/PointLight.h"

PointLight::PointLight(Graphics& gfx) : Light::Light(gfx,"point_light.obj")
{
	attritute_.light_type = 0.f;
	attritute_.affect_radius_ = 20.f;
}

LightSet PointLight::GetLightAttribute()
{
	attritute_.light_pos_.x = world_location_.x;
	attritute_.light_pos_.y = world_location_.y;
	attritute_.light_pos_.z = world_location_.z;
	return attritute_;
}

void PointLight::SetRadius(float radius)
{
	attritute_.affect_radius_ = radius;
}
