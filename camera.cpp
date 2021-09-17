#include "camera.h"
#include <string>
#include "Utili.h"
#include "QDebug"
#include "QString"

Camera::Camera()
{
	this->location_f_ = DirectX::XMFLOAT3(0.0f, 0.0f, -50.0f);
	this->location_v_ = DirectX::XMLoadFloat3(&this->location_f_);
	this->rotation_f_ = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->rotation_v_ = DirectX::XMLoadFloat3(&this->rotation_f_);
	this->UpdateViewMatrix();
}

void Camera::SetProjection(float fov_degrees, float aspect_ratio, float near_z, float far_z)
{
	float fov_radians = (fov_degrees / 360.0f) * DirectX::XM_2PI;
	this->projection_matrix_ = DirectX::XMMatrixPerspectiveFovLH(fov_radians, aspect_ratio, near_z, far_z);
}

const DirectX::XMMATRIX Camera::view_matrix() const
{
	return this->view_matrix_;
}

const DirectX::XMMATRIX Camera::projection_matrix() const
{
	return this->projection_matrix_;
}

const DirectX::XMVECTOR Camera::location_v() const
{
	return this->location_v_;
}

const DirectX::XMVECTOR Camera::roation_v() const
{
	return this->rotation_v_;
}

const DirectX::XMFLOAT3 Camera::location_f() const
{
	return this->location_f_;
}

const DirectX::XMFLOAT3 Camera::rotation_f() const
{
	return this->rotation_f_;
}

const XMVECTOR Camera::forward() const
{
	return forward_;
}

const XMVECTOR Camera::right() const
{
	return right_;
}

const DirectX::XMVECTOR Camera::up() const
{
	return up_;
}

void Camera::SetLocation(const DirectX::XMVECTOR& pos)
{
	DirectX::XMStoreFloat3(&this->location_f_, pos);
	this->location_v_ = pos;
	this->UpdateViewMatrix();
}

void Camera::SetLocation(float x, float y, float z)
{
	this->location_f_ = DirectX::XMFLOAT3(x, y, z);
	this->location_v_ = DirectX::XMLoadFloat3(&this->location_f_);
	this->UpdateViewMatrix();
}

void Camera::AddPosition(const DirectX::XMVECTOR& pos)
{
	this->location_v_ += pos;
	DirectX::XMStoreFloat3(&this->location_f_, this->location_v_);
	this->UpdateViewMatrix();
}

void Camera::AddPosition(float x, float y, float z)
{
	this->location_f_.x += x;
	this->location_f_.y += y;
	this->location_f_.z += z;
	this->location_v_ = XMLoadFloat3(&this->location_f_);
	this->UpdateViewMatrix();
}

void Camera::SetRotation(const DirectX::XMVECTOR& rot)
{
	DirectX::XMStoreFloat3(&this->rotation_f_, rot);
	this->rotation_v_ = rot;
	this->UpdateViewMatrix();
}

void Camera::SetRotation(float x, float y, float z)
{
	this->rotation_f_ = DirectX::XMFLOAT3(x, y, z);
	this->rotation_v_ = DirectX::XMLoadFloat3(&this->rotation_f_);
	this->UpdateViewMatrix();
}

void Camera::AddRotation(const DirectX::XMVECTOR& rot)
{
	this->rotation_v_ += rot;
	XMStoreFloat3(&this->rotation_f_, this->rotation_v_);
	this->UpdateViewMatrix();
}

void Camera::AddRotation(float x, float y, float z)
{
	this->rotation_f_.x += x;
	this->rotation_f_.y += y;
	this->rotation_f_.z += z;
	this->rotation_v_ = DirectX::XMLoadFloat3(&this->rotation_f_);
	this->UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	//Calculate camera rotation matrix
	DirectX::XMMATRIX cam_rotation_matrix = DirectX::XMMatrixRotationRollPitchYaw(this->rotation_f_.x, this->rotation_f_.y, this->rotation_f_.z);
	//Calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
	DirectX::XMVECTOR cam_target = XMVector3TransformCoord(this->kDefaultForwardVector_, cam_rotation_matrix);
	//Adjust cam target to be offset by the camera's current position
	cam_target += this->location_v_;
	//Calculate up direction based on current rotation
	DirectX::XMVECTOR upDir = XMVector3TransformCoord(this->kDefaultUpVector_, cam_rotation_matrix);
	//Rebuild view matrix
	this->view_matrix_ =XMMatrixLookAtLH(this->location_v_, cam_target, upDir);

	//calcalate direction
	auto matrix = XMMatrixRotationRollPitchYaw(rotation_f_.x, rotation_f_.y,0.f);
	this->forward_ = XMVector3Transform(kDefaultForwardVector_, matrix);
	this->right_ = XMVector3Transform(kDefaultRightVector_, matrix);
	this->up_ = XMVector3Transform(kDefaultUpVector_, matrix);
}
