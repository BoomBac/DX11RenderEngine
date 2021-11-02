#ifndef DX11ENGINE_RENDER_EFFECT_PASS_H
#define DX11ENGINE_RENDER_EFFECT_PASS_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

#include <Public/Render/Bindable/PixelShader.h>
#include <Public/Render/Bindable/VertexShader.h>

class Pass
{
public:
    Pass() = default;
    void Render(const std::vector<Drawable*>& render_object);
    void SetRTVonly(ID3D11RenderTargetView* view);
    void SetDASonly(ID3D11DepthStencilView* depth);
    void SetRTVandDAS(ID3D11RenderTargetView* view, ID3D11DepthStencilView* depth);
    void AddVertexShader(const char* file_name);
    void AddPixelShader(const char* file_name);
    void SetShadeResource(ID3D11ShaderResourceView** view,UINT id);
    void AddDepthState(const D3D11_DEPTH_STENCIL_DESC& des);
    void AddBlendState(const D3D11_RENDER_TARGET_BLEND_DESC& des);
    bool b_shadow_pass = false;
private:
    DISALLOW_COPY_AND_ASSIGN(Pass)
    void UpdateBuffer();
    void BeginPass();
    void ApplyPass();
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> p_depth_state_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D11BlendState> p_blend_state = nullptr;
    std::shared_ptr<PixelShader> p_pixel_shader_ = nullptr;
    std::shared_ptr<VertexShader> p_vertex_shader_ = nullptr;

    //ID3D11RenderTargetView* 
    //ID3D11DepthStencilView*
};


#endif //DX11ENGINE_RENDER_EFFECT_PASS_H
