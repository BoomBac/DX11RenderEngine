#ifndef DX11ENGINE_RENDER_MATERIAL_MATERIAL_H
#define DX11ENGINE_RENDER_MATERIAL_MATERIAL_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

#include <Public/Render/Graphics.h>
#include <Public/Render/Bindable/PixelShader.h>

struct CBufferValueInfo
{
    UINT slot;
    UINT offset;
};
//enum class ETextureType : BYTE
//{
//    kDiffuse,
//    kNormal,
//    kMetallic,
//    kAlbedo,
//    kRoughness,
//    kAmbientOcclusion,
//    kIrradiance,
//    kSpecularMap,
//    kLUT
//};

class Material
{
public:
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
    void InitTexture();
    void CommitAllBufferData();
private:
    DISALLOW_COPY_AND_ASSIGN(Material)
    std::shared_ptr<PixelShader> p_shader_;

    std::map<std::string, UINT> texture_struct_;
    std::map<std::string, UINT> sampler_struct_;
    //map container every value in cbuf; <value_name : (buf_slot,offset)>
    std::map<std::string,CBufferValueInfo> cval_info_;
    // vector that container the <slot£ºcbuf/gbuf>
    std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> p_gbuffer_;
    std::vector<void*> p_cbuffer_;
    // vector that container the <slot£ºbuf_size>
    std::vector<int> buf_size_;
};

#endif //DX11ENGINE_RENDER_MATERIAL_MATERIAL_H
