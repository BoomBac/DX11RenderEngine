#ifndef DX11ENGINE_RENDER_LIGHT_DIRECTIONALLIGHT_H
#define DX11ENGINE_RENDER_LIGHT_DIRECTIONALLIGHT_H
#include "Light.h"

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif


class DirectionalLight : public Light
{
public:
	DirectionalLight() = delete;
	DirectionalLight(Graphics& gfx);
	virtual void UpdateAttribute() override;
private:
	DISALLOW_COPY_AND_ASSIGN(DirectionalLight)
	DirectX::XMFLOAT3 direction_;
};



#endif //DX11ENGINE_RENDER_LIGHT_DIRECTIONALLIGHT_H
