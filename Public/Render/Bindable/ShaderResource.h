#ifndef DX11ENGINE_RENDER_BINDABLE_SHADERRESOURCE_H
#define DX11ENGINE_RENDER_BINDABLE_SHADERRESOURCE_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif


#include "Public/Render/Bindable/BindableInterface.h"

enum class ETextureType : BYTE
{
    kDiffuse,
    kNormal,
    kMetallic,
    kAlbedo,
    kRoughness,
    kAmbientOcclusion,
    kIrradiance
};

class ShaderResource : public BindableInterface
{
public:
    ShaderResource(ID3D11ShaderResourceView* srv,ETextureType type);
    virtual void Bind(Graphics& gfx) override;
    void SetTexture(ID3D11ShaderResourceView* srv, ETextureType type);
    ID3D11ShaderResourceView* GetResourceView();
    ETextureType GetTextureType() const;
    virtual EBindableType GetType() const override;
private:
    DISALLOW_COPY_AND_ASSIGN(ShaderResource)
    ETextureType tex_type_;
    ID3D11ShaderResourceView* p_shader_res_v_ = nullptr;
};

#endif //DX11ENGINE_RENDER_BINDABLE_SHADERRESOURCE_H
