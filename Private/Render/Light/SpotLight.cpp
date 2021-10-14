#include "Public/Render/Light/SpotLight.h"
//spot_light.obj

SpotLight::SpotLight(Graphics &gfx):Light::Light(gfx, "spot_light.obj")
{
	inner_angle_ = 30.f;
	outer_angle_ = 45.f;
	attritute_.outer_angle = outer_angle_;
	attritute_.inner_angle_ = inner_angle_;
	direction_ = { 0.f,-1.f,0.f };
	attritute_.light_type = 2.f;
	attritute_.light_dir_ = direction_;
	attritute_.affect_radius_ = 20.f;
	light_type_ = ELightType::kSpotLight;
}

void SpotLight::UpdateAttribute()
{
	//direction_ = { 0.f,-1.f,0.f };
	DirectX::XMStoreFloat3(&direction_, light_camera_.forward());
	//DirectX::XMStoreFloat3(&direction_, DirectX::XMVector3TransformNormal(XMLoadFloat3(&direction_), GetRotationMartix()));

	attritute_.light_dir_ = direction_;
	attritute_.light_pos_ = light_camera_.location_f();
	//attritute_.light_pos_.x = world_location_.x;
	//attritute_.light_pos_.y = world_location_.y;
	//attritute_.light_pos_.z = world_location_.z;
}

void SpotLight::SetInnerAngle(float angle)
{
	attritute_.inner_angle_ = angle;
}

void SpotLight::SetOuterAngle(float angle)
{
	attritute_.outer_angle = angle;
}
