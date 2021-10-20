#include "Public/Render/Light/PointLight.h"

PointLight::PointLight(Graphics& gfx) : Light::Light(gfx,"point_light.obj")
{
	attritute_.light_type = 0.f;
	attritute_.affect_radius_ = 20.f;
	light_type_ = ELightType::kPonintLight;
}

void PointLight::UpdateAttribute()
{
	//SetWorldLocation({ attritute_.light_pos_.x,attritute_.light_pos_.y,attritute_.light_pos_.z });
	light_camera_.SetLocation(world_location_.x, world_location_.y, world_location_.z);
	attritute_.light_pos_ = light_camera_.location_f();
}

void PointLight::SetRadius(float radius)
{
	attritute_.affect_radius_ = radius;
}
