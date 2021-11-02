#ifndef DX11ENGINE_RENDER_BINDABLE_SHADERRESOURCE_H
#define DX11ENGINE_RENDER_BINDABLE_SHADERRESOURCE_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif


#include "Public/Render/Bindable/BindableInterface.h"

enum class ESResourceType : BYTE
{
    kDiffuse,
    kNormal,
    kMetallic,
    kAlbedo,
    kRoughness,
    kAmbientOcclusion,
    kIrradiance,
    kSpecularMap,
    kLUT
};

class ShaderResource : public BindableInterface
{
public:
    ShaderResource(ID3D11ShaderResourceView* srv,ESResourceType type);
    virtual void Bind(Graphics& gfx) override;
    void SetTexture(ID3D11ShaderResourceView* srv, ESResourceType type);
    ID3D11ShaderResourceView* GetResourceView();
    ESResourceType GetTextureType() const;
    virtual EBindableType GetType() const override;
private:
    DISALLOW_COPY_AND_ASSIGN(ShaderResource)
    ESResourceType tex_type_;
    ID3D11ShaderResourceView* p_shader_res_v_ = nullptr;
};

#endif //DX11ENGINE_RENDER_BINDABLE_SHADERRESOURCE_H
