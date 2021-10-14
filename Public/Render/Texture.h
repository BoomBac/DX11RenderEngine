#ifndef DX11ENGINE_RENDER_TEXTURE_H
#define DX11ENGINE_RENDER_TEXTURE_H
#include <d3d11.h>
#include <wrl/module.h>

#include "Public/Render/Bindable/BindableInterface.h"

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

class Texture 
{
public:
    Texture(ID3D11Device* device);
    Texture(ID3D11Device* device, const char* file_path);
    void Load(const char* file_path);
	ID3D11ShaderResourceView* GetTextureResourceView();
	ID3D11ShaderResourceView** GetTextureResourceViewAddress();
private:
    DISALLOW_COPY_AND_ASSIGN(Texture)
    static ID3D11Device* device_;
	Microsoft::WRL::ComPtr<ID3D11Resource> p_texture_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> p_textureView_ = nullptr;
};


#endif //DX11ENGINE_RENDER_TEXTURE_H
