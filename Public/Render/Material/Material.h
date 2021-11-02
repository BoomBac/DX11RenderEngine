#ifndef DX11ENGINE_RENDER_MATERIAL_MATERIAL_H
#define DX11ENGINE_RENDER_MATERIAL_MATERIAL_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

#include <array>

#include <Public/Render/Graphics.h>
#include <Public/Render/Bindable/PixelShader.h>
#include "Public/Render/Texture.h"

struct CBufferValueInfo
{
    UINT slot;
    UINT offset;
};


class Material
{
public:
    enum ESResourceType : UINT
    {
        kAlbedo = 0,
        kMetallic,
        kRoughness,
        kNormal,
        kAmbientOcclusion,
    };

    Material() = default;

    void LoadFromLib(const std::string& name);
    void Bind();
    ~Material();
    //set data
    bool SetFloat(const std::string& name, const float& value);
    bool SetFloat3(const std::string& name, const DirectX::XMFLOAT3& value);
    bool SetFloat4(const std::string& name, const DirectX::XMFLOAT4& value);
    bool SetMatrix4x4(const std::string& name, const DirectX::XMMATRIX& value);
    bool SetTextureByIndex(const UINT& id, const std::string& tex_name);
    bool SetTextureByName(const std::string& name, const std::string& tex_name);
    //init texture
    void InitTexture(const std::string& name,ESResourceType tex_type);
    //init textures,must pass by albedo->metallic->roughness->normal->ao
    void InitTexture(const std::vector<std::string>& name_list);
    void CommitAllBufferData();
    void CommitAllTexture();

    //apply all the input to the pipeline
    void CommitAllInput();
private:
    DISALLOW_COPY_AND_ASSIGN(Material)
    std::shared_ptr<PixelShader> p_shader_;

    std::map<std::string, UINT> texture_struct_;
    std::map<std::string, UINT> sampler_struct_;

    std::map<ESResourceType, std::string> texture_list_;
    //map container every value in cbuf; <value_name : (buf_slot,offset)>
    std::map<std::string,CBufferValueInfo> cval_info_;
    // vector that container the <slot£ºcbuf/gbuf>
    std::array<Microsoft::WRL::ComPtr<ID3D11Buffer>, 16u> p_gbuffer_;
    std::array<void*, 16u> p_cbuffer_;
    // vector that container the <slot£ºbuf_size>
    std::array<int, 16u> buf_size_;
    UINT buffer_num = 0u;

    std::array<std::shared_ptr<Texture>, 5u> textures_;
    UINT texture_num_ = 0u;
};

#endif //DX11ENGINE_RENDER_MATERIAL_MATERIAL_H
