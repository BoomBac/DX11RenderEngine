#include "Public/Render/Light/DirectionalLight.h"

DirectionalLight::DirectionalLight(Graphics& gfx) : Light::Light(gfx, "directional_light.obj")
{
	attritute_.light_type = 1.f;
	direction_ = { 0.f,-1.f,0.f };
	light_type_ = ELightType::kDirectionLight;
}

void DirectionalLight::UpdateAttribute()
{
	DirectX::XMStoreFloat3(&direction_, light_camera_.forward());
	SetWorldLocation({ 0.f,100.f,0.f });
	//qDebug() << (static_cast<int>(RadToDeg(light_camera_.rotation_f().x))/10);
	//SetActorRotation({ -RadToDeg(light_camera_.rotation_f().x) /10.f + 90.f ,-RadToDeg(light_camera_.rotation_f().z) / 10.f +90.f ,0.f});
	//SetWorldRotation({ light_camera_.rotation_f().x ,light_camera_.rotation_f().y,light_camera_.rotation_f().z });
	attritute_.light_dir_ = direction_;
}

