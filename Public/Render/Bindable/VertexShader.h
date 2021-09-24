#ifndef DX11ENGINE_RENDER_BINDABLE_VERTEXSHADER_H
#define DX11ENGINE_RENDER_BINDABLE_VERTEXSHADER_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif


#include "BindableInterface.h"


class VertexShader :
    public BindableInterface
{
    friend class InputLayout;
public:
    VertexShader(Graphics& gfx,const std::string& path);
    virtual void Bind(Graphics& gfx) override;
    virtual EBindableType GetType() const override;
private:
    DISALLOW_COPY_AND_ASSIGN(VertexShader)
    Microsoft::WRL::ComPtr<ID3D11VertexShader> p_vertx_shader_ = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> p_blob_ = nullptr;
};

#endif //DX11ENGINE_RENDER_BINDABLE_VERTEXSHADER_H
