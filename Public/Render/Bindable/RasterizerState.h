#ifndef DX11ENGINE_RENDER_BINDABLE_RASTERIZERSTATE_H
#define DX11ENGINE_RENDER_BINDABLE_RASTERIZERSTATE_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

#include "./BindableInterface.h"

class RasterizerState : public BindableInterface
{
public:
    RasterizerState(Graphics& gfx, const D3D11_RASTERIZER_DESC& des);
    RasterizerState(Graphics& gfx);
    virtual void Bind(Graphics& gfx) override;
    virtual EBindableType GetType() const override;
private:
    DISALLOW_COPY_AND_ASSIGN(RasterizerState)
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> p_raster_state_;
};

#endif // !DX11ENGINE_RENDER_BINDABLE_RASTERIZERSTATE_H