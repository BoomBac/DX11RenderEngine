#ifndef DX11ENGINE_RENDER_LIGHT_LIGHT_H
#define DX11ENGINE_RENDER_LIGHT_LIGHT_H

#include "../Shape/Model.h"
#include "../Graphics.h"


#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

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

class Light : public Model
{
public:
    Light() = delete;
    Light(Graphics& gfx,const char* light_type);
    virtual LightSet GetLightAttribute();
    void SetColor(DirectX::XMFLOAT4 color);
    void SetIntensity(float intensity);
private:
    DISALLOW_COPY_AND_ASSIGN(Light)
protected:
    LightSet attritute_;
};



#endif //DX11ENGINE_RENDER_LIGHT_LIGHT_H