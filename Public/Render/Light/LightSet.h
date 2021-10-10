#include <DirectXMath.h>

struct LightSet
{
	DirectX::XMFLOAT3 light_pos_;
	float light_intensity_;
	DirectX::XMFLOAT3 light_dir_;
	float padding;
	DirectX::XMFLOAT4 light_color_;
	float affect_radius_;
	float inner_angle_;
	float outer_angle;
	float light_type;
};

struct LightShader
{
	DirectX::XMFLOAT3 light_color_;
	float light_intensity_;
};