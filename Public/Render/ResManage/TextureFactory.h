#ifndef DX11ENGINE_RENDER_RESMANAGE_TEXTUREFACTORY_H
#define DX11ENGINE_RENDER_RESMANAGE_TEXTUREFACTORY_H

#include <map>
#include <string>

#include <Public/Render/Texture.h>
#include <memory>
#include <Public/Render/Graphics.h>
#include "ResManage.h"


#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif 

class TextureFactory : public ResManage
{
public:
	static TextureFactory& GetInstance();
	void AddTexture(std::string file_path);
	std::shared_ptr<Texture> GetTexture(std::string file_name);
private:
	DISALLOW_COPY_AND_ASSIGN(TextureFactory)
	TextureFactory() {};
	std::map<std::string, std::shared_ptr<Texture>> texture_pool_;
};

#endif//DX11ENGINE_RENDER_RESMANAGE_TEXTUREFACTORY_H