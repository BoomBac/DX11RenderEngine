#ifndef DX11ENGINE_RENDER_LIGHT_SPOTLIGHT_H
#define DX11ENGINE_RENDER_LIGHT_SPOTLIGHT_H

#include "Light.h"

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

class SpotLight : public Light
{
public:
	SpotLight() = delete;
	SpotLight(Graphics& gfx);
	virtual LightSet GetLightAttribute() override;
	void SetInnerAngle(float angle);
	void SetOuterAngle(float angle);
private:
	DISALLOW_COPY_AND_ASSIGN(SpotLight)
	float inner_angle_;
	float outer_angle_;
	DirectX::XMFLOAT3 direction_;
};



#endif //DX11ENGINE_RENDER_LIGHT_SPOTLIGHT_H