#ifndef DX11ENGINE_RENDER_LIGHT_LIGHT_H
#define DX11ENGINE_RENDER_LIGHT_LIGHT_H

#include "../Shape/Model.h"
#include "../Graphics.h"
//#include "LightSet.h"


#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif



class Light : public Model
{
public:
    Light() = delete;
    Light(Graphics& gfx,const char* light_type);
    virtual void UpdateAttribute();
    void SetColor(DirectX::XMFLOAT4 color);
    void SetIntensity(float intensity);
    virtual void Draw(Graphics& gfx) override;
    LightShader raw_shader_;
    ELightType GetLightType() const;
    LightSet* GetAttritute();
    LightMatrix* GetLightMatrix();
    Camera light_camera_;
    void UpdateLightMatrix();
private:
    DISALLOW_COPY_AND_ASSIGN(Light)
protected:
    LightSet attritute_;
    ELightType light_type_;
    LightMatrix matrix_;
    //DirectX::XMMATRIX* p_view_;
    //DirectX::XMMATRIX* p_projection_;
    //DirectX::XMMATRIX* p_view_projection_;
};



#endif //DX11ENGINE_RENDER_LIGHT_LIGHT_H