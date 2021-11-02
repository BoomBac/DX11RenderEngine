#include "Public/Render/Light/Light.h"



Light::Light(Graphics& gfx, const char* light_type) : Model::Model(gfx, light_type)
{
	attritute_.light_intensity_ = 0.4f;
	attritute_.light_color_ = {1.f,1.f,1.f,1.f};
	raw_shader_.light_intensity_ = 1.f;
	raw_shader_.light_color_ = {1.f,1.f,1.f};
	light_camera_.SetProjection(75.f, 4.f / 3.f, 1.f, 1000.f);
	light_camera_.SetLocation({ 0.f, 50.f, 50.f,0.f });
	light_camera_.SetRotation(DegToRad(60.f), DegToRad(180.f), 0.f);
	gfx.p_scene_light_ = &attritute_;
	gfx.p_light_shader_ = &raw_shader_;
	cast_shadow_ = false;
	UpdateLightMatrix();
	effects_.clear();
	effects_.push_back(EEffectType::kNone);
}

void Light::UpdateAttribute()
{
	matrix_.porjection = light_camera_.projection_matrix();
	matrix_.view = light_camera_.view_matrix();
	matrix_.view_porjection = light_camera_.view_projection_matrix();
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

LightMatrix* Light::GetLightMatrix()
{
	return &matrix_;
}

void Light::UpdateLightMatrix()
{
	matrix_.porjection = light_camera_.projection_matrix();
	matrix_.view = light_camera_.view_matrix();
	matrix_.view_porjection = light_camera_.view_projection_matrix();
}

