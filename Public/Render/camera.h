#pragma once
#include <DirectXMath.h>

using namespace DirectX;

enum class ECameraMovementState : char
{
	kForward,
	kBack,
	kRight,
	kLeft,
	kUp,
	kDown,
	kStop
};

class Camera
{
public:
	Camera();
	void SetProjection(float fov_degrees, float aspect_ratio, float near_z, float far_z);

	const DirectX::XMMATRIX view_matrix() const;
	const DirectX::XMMATRIX projection_matrix() const;
	const DirectX::XMVECTOR location_v() const;
	const DirectX::XMVECTOR roation_v() const;
	const DirectX::XMFLOAT3 location_f() const;
	const DirectX::XMFLOAT3 rotation_f() const;
	const XMVECTOR forward() const;
	const XMVECTOR right() const;
	const XMVECTOR up() const;

	void SetLocation(const DirectX::XMVECTOR& pos);
	void SetLocation(float x, float y, float z);
	void AddPosition(const DirectX::XMVECTOR& pos);
	void AddPosition(float x, float y, float z);
	void SetRotation(const DirectX::XMVECTOR& rot);
	void SetRotation(float x, float y, float z);
	void AddRotation(const DirectX::XMVECTOR& rot);
	void AddRotation(float x, float y, float z);

	
private:
	void UpdateViewMatrix();
	DirectX::XMMATRIX view_matrix_;
	DirectX::XMMATRIX projection_matrix_;
	DirectX::XMVECTOR location_v_;
	DirectX::XMVECTOR rotation_v_;
	DirectX::XMFLOAT3 location_f_;
	DirectX::XMFLOAT3 rotation_f_;

	const DirectX::XMVECTOR kDefaultForwardVector_ = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR kDefaultRightVector_ = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR kDefaultUpVector_ = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//当前摄像机各方向
	XMVECTOR forward_;
	XMVECTOR right_;
	XMVECTOR up_;
};

