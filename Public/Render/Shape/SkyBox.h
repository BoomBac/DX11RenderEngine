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
private:
    DISALLOW_COPY_AND_ASSIGN(SkyBox)
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> p_srv_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> p_hdr_srv_;
    std::unique_ptr<Texture> tex_top;
    std::unique_ptr<Texture> tex_bottom;
    std::unique_ptr<Texture> tex_right;
    std::unique_ptr<Texture> tex_left;
    std::unique_ptr<Texture> tex_front;
    std::unique_ptr<Texture> tex_back;
    std::vector<Texture*> tex_vec_;
    std::vector<std::string> image_paths_;
    void GenerateCubeMap();
    void TexturedFromCube();
    void TexturedFromEquirectangular();
};

#endif // !DX11ENGINE_RENDER_SHAPE_SKYBOX_H

