#include <DirectXMath.h>

enum class ELightType : BYTE
{
	kPonintLight,
	kDirectionLight,
	kSpotLight
};

struct LightSet
{
	float light_intensity_;
	DirectX::XMFLOAT3 light_pos_;
	DirectX::XMFLOAT3 light_dir_;
	float padding;
	DirectX::XMFLOAT4 light_color_;
	float affect_radius_;
	float inner_angle_;
	float outer_angle;
	//0:point 1:direction 2:spot
	float light_type;
};

struct LightMatrix
{
	DirectX::XMMATRIX view_porjection;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX porjection;
};

struct LightShader
{
	DirectX::XMFLOAT3 light_color_;
	float light_intensity_;
};