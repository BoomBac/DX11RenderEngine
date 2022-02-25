#include "Public/Render/ResManage/TextureFactory.h"
#include "QFileInfo"


namespace
{
	const char* common_dir_ = "E:/Project_VS2019/DX11RenderEngine/Res/Texture/";
}

TextureFactory& TextureFactory::TextureFactory::GetInstance()
{
	static TextureFactory tex_fac;
	return tex_fac;
}

void TextureFactory::AddTexture(std::string file_path)
{
	std::string path = std::string(common_dir_).append(file_path);
	QFileInfo fin(QString::fromStdString(path));
	texture_pool_.insert(std::pair<std::string, std::shared_ptr<Texture >>(fin.fileName().toStdString().c_str(), std::make_shared<Texture>(gfx_->GetDevice(), path.c_str())));
}

std::shared_ptr<Texture> TextureFactory::GetTexture(std::string file_name)
{
	return texture_pool_.find(file_name)->second;
}


