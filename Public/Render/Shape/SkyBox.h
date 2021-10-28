#ifndef DX11ENGINE_RENDER_SHAPE_SKYBOX_H
#define DX11ENGINE_RENDER_SHAPE_SKYBOX_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

#include "Public\Render\Drawable\Drawable.h"
#include "Public\Render\Shape\Shape.hpp"
#include <Public/Render/Texture.h>

enum class EGenerateFlag
{
    kEnvironment,
    kIrradiance,
    kSpecular
};

struct PixelBuffer
{
    float roughness;
    float padding0;
    float padding1;
    float padding2;
};

/// <summary>
/// * load sdr image(single-sphere/six-cube) for shading
/// * load hdr sphere image and generate cube edition and fliter it
/// </summary>
class SkyBox : public Shape<SkyBox>
{
public:
	SkyBox(Graphics& gfx);
    ID3D11ShaderResourceView* GetEnvironmentRsv() const;
    //create 6 cube_maps from sphere to the file
    void GenerateCubeSurface(const UINT& size,const std::vector<std::string>& path_list);
    //create 6 cube_maps from sphere to the shaderResource
    void GenerateCubeSurface(const UINT& size, EGenerateFlag map_type);
    //create specular surface
    void GenerateSpecularSurface();
    // create 6 cube surface for irrandice,must be called after GenerateCubeSurface(env/specular)
    void GenerateIrrandiceSurface(const UINT& size);
    //create mip-map cube for the specular
    void GenerateSpecularCube();
    //shading the skyBox with the shaderresource,must call after GenerateCube,sample from cube
    void ShaderingFromResource(EGenerateFlag map_type);
    //load a sphere_image for shading
    void ShaderingFromFile(const std::string& sphere_image_path);
    //load six cube_map for shading
    void ShaderingFromFile(const std::vector<std::string>& cube_image_paths, const UINT& width, const UINT& height);
    //bind cube_map to ShaderResourceView at slot 6
    void BindToShadering(EGenerateFlag env_type);

    void LoadLUT(const std::string& path);
private:
    DISALLOW_COPY_AND_ASSIGN(SkyBox)
    UINT irradiance_map_size_;
    UINT environment_map_size_;
    UINT cube_map_level = 11;
    UINT max_roughness_level_ = 5;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> p_env_cube_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> p_hdr_cube_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> p_specular_cube_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> p_LUT_map_;
    D3D11_TEXTURE2D_DESC face_desc;
    std::vector<std::string> image_paths_;
    //rl ub fb
    std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> texture_res_;
    //rl ub fb
    std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> irradiance_res_;
    std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> specular_res_;
    void InitBindable(Graphics& gfx);
    //generate texture_cube ,must call after the GenerateCubeSurface
    void GenerateCube(EGenerateFlag cube_type);
    PixelBuffer* p_roughness_ = &roughness_;
    //for the pxiel conf
    PixelBuffer roughness_;
};

#endif // !DX11ENGINE_RENDER_SHAPE_SKYBOX_H

