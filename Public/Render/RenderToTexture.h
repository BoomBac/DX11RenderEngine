#ifndef DX11ENGINE_RENDER_RENDERTOTEXTURE_H
#define DX11ENGINE_RENDER_RENDERTOTEXTURE_H
#include <wrl/module.h>
#include <d3d11.h>
#include "Graphics.h"


#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

enum class ERTTUsage : BYTE
{
    kBackBuffer,
    kDepthBuffer
};

class RenderToTexture
{
public:
    RenderToTexture();
    void Initialize(Graphics* gfx, ERTTUsage usage);
	void SetRenderTarget(Graphics* gfx);
	void ClearRenderTarget(Graphics* gfx,float red, float green, float blue, float alpha);
//	void ClearRenderTarget(Graphics* gfx,UINT red, UINT green, UINT blue, UINT alpha);
    void SaveToImage(Graphics* gfx,std::string path);
    void SaveToImage(Graphics* gfx);
    std::pair<Microsoft::WRL::ComPtr<ID3D11Texture2D>, D3D11_TEXTURE2D_DESC> MakeStaging(Graphics* gfx);
    ERTTUsage usage_;

    ID3D11ShaderResourceView** GetShaderResourceView();
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> p_texture_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> p_target_view_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> p_shader_view_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> p_depth_view = nullptr;
    D3D11_VIEWPORT p_view_port;
};

#endif //DX11ENGINE_RENDER_RENDERTOTEXTURE_H
