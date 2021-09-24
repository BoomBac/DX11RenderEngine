#ifndef DX11ENGINE_RENDER_BINDABLE_INPUTLAYOUT_H
#define DX11ENGINE_RENDER_BINDABLE_INPUTLAYOUT_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

#include "BindableInterface.h"
#include "VertexShader.h"
#include "Public/Render/VertexLayout.h"


class InputLayout :
    public BindableInterface
{
public:
    InputLayout(Graphics& gfx, const VertexShader& vs, VertexLayout& vl,D3D11_PRIMITIVE_TOPOLOGY topology);
    virtual void Bind(Graphics& gfx) override;
    virtual EBindableType GetType() const override;
private:
    DISALLOW_COPY_AND_ASSIGN(InputLayout)
    Microsoft::WRL::ComPtr<ID3D11InputLayout> p_input_layout_ = nullptr;
    D3D11_PRIMITIVE_TOPOLOGY topology_;
};

#endif //DX11ENGINE_RENDER_BINDABLE_INPUTLAYOUT_H
