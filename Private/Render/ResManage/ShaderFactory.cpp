#include "Public/Render/ResManage/ShaderFactory.h"
#include "Public/Global.h"


namespace 
{
	constexpr char* common_dir_ = "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/";
}

ShaderFactory& ShaderFactory::getInstance()
{
	static ShaderFactory fac;
	return fac;
}

void ShaderFactory::AddShader(std::string file_name, EShaderType type)
{
	if(type == EShaderType::kPixelShader)
		pshader_lib_.insert(std::move(std::make_pair(file_name,
		std::move(std::make_shared<PixelShader>(*Global::getInstance()->G_Gfx, std::string(common_dir_) + file_name)))));
	else if(type == EShaderType::kVertexShader)
		vshader_lib_.insert(std::move(std::make_pair(file_name,
			std::move(std::make_shared<VertexShader>(*Global::getInstance()->G_Gfx, std::string(common_dir_) + file_name)))));
}

std::shared_ptr<PixelShader> ShaderFactory::GetPShader(std::string file_name)
{
	auto it = pshader_lib_.find(file_name);
	if (it == pshader_lib_.end())
		AddShader(file_name, EShaderType::kPixelShader);
	else
		return it->second;
	return pshader_lib_.find(file_name)->second;
}

std::shared_ptr<VertexShader> ShaderFactory::GetVShader(std::string file_name)
{
	auto it = vshader_lib_.find(file_name);
	if (it == vshader_lib_.end())
		AddShader(file_name, EShaderType::kVertexShader);
	else
		return it->second;
	return vshader_lib_.find(file_name)->second;
}

