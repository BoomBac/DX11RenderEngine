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

class SkyBox : public Shape<SkyBox>
{
public:
	SkyBox(Graphics& gfx);
    ID3D11ShaderResourceView* GetEnvironmentRsv() const;
    //create 6 cube_maps from sphere to the file
    void GenerateCubeMap(const UINT& width, const UINT& height,const std::vector<std::string>& path_list);
    //create 6 cube_maps from sphere to the RenderResource
    void GenerateCubeMap(const UINT& width, const UINT& height);
    //using the 6 cube_map resource created inside
    void ShaderingFromResource();
    //load a sphere hdr image for shading
    void ShaderingFromFile(const std::string& sphere_image_path);
    //load six cube_map for shading
    void ShaderingFromFile(const std::vector<std::string>& cube_image_paths, const UINT& width, const UINT& height);
private:
    DISALLOW_COPY_AND_ASSIGN(SkyBox)
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> p_srv_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> p_hdr_srv_;
    D3D11_TEXTURE2D_DESC face_desc;
    std::vector<std::string> image_paths_;
    // fb lr tb
    std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> texture_res_;
    UINT width_;
    UINT height_;
};

#endif // !DX11ENGINE_RENDER_SHAPE_SKYBOX_H

