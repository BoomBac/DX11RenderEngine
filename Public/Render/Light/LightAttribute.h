#ifndef DX11ENGINE_RENDER_LIGHT_LIGHTATTRIBUTE_H
#define DX11ENGINE_RENDER_LIGHT_LIGHTATTRIBUTE_H
#include <DirectXMath.h>

struct LightSet
{
	DirectX::XMFLOAT3 light_pos_;
	float light_intensity_;
	DirectX::XMFLOAT4 light_color_;
	float affect_radius_;
	float inner_angle_;
	float outer_angle;
	float padding;
};


#endif //DX11ENGINE_RENDER_LIGHT_LIGHTATTRIBUTE_H