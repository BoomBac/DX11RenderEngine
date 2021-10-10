#ifndef DX11ENGINE_RENDER_LIGHT_POINTLIGHT_H
#define DX11ENGINE_RENDER_LIGHT_POINTLIGHT_H

#include "Light.h"

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

class PointLight : public Light
{
public:
	PointLight() = delete;
	PointLight(Graphics& gfx);
	virtual void UpdateAttribute() override;
	void SetRadius(float radius);
private:
	DISALLOW_COPY_AND_ASSIGN(PointLight)
};



#endif //DX11ENGINE_RENDER_LIGHT_POINTLIGHT_H