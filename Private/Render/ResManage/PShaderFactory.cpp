#include "Public/Render/ResManage/PShaderFactory.h"
#include "Public/Global.h"


namespace 
{
	constexpr char* common_dir_ = "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/";
}

PShaderFactory& PShaderFactory::getInstance()
{
	static PShaderFactory fac;
	return fac;
}

void PShaderFactory::AddShader(std::string file_name)
{
	shader_lib_.insert(std::move(std::make_pair(file_name,
		std::move(std::make_shared<PixelShader>(*Global::getInstance()->G_Gfx, std::string(common_dir_) + file_name)))));
}

std::shared_ptr<PixelShader> PShaderFactory::GetShader(std::string file_name)
{
	auto it = shader_lib_.find(file_name);
	if (it == shader_lib_.end())
		AddShader(file_name);
	else
		return it->second;
	return shader_lib_.find(file_name)->second;
}

