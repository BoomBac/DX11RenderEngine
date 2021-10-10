#include <utility>

#include "vector3d.h"
#include "vector2d.h"

#include "Public/Render/ModelResFactory.h"
#include "Public/Render/Bindable/VertexBuffer.hpp"
#include "Public/Render/VertexLayout.h"
#include "Public/Render/Bindable/IndexBuffer.h"
#include "Public/Render/Bindable/VertexShader.h"
#include "Public/Render/Bindable/PixelShader.h"
#include "Public/Render/Bindable/InputLayout.h"
#include <Public/Render/ResManage/MeshFactory.h>
#include "Public/Render/Bindable/LightBuffer.h"
#include "Public/Render/Light/DirectionalLight.h"






using namespace std;


template<typename T>
using Vec = std::vector<T, std::allocator<T>>;
using BindItem = std::shared_ptr<BindableInterface>;



inline bool ModelResFactory::Exist(std::string model_key)
{
	return res_pool_.find(model_key) != res_pool_.end();
}

inline bool ModelResFactory::Exist(std::string model_key, std::map<std::string, std::vector<std::shared_ptr<BindableInterface>>>::iterator& it)
{
	it = res_pool_.find(model_key);
	return it != res_pool_.end();
}


ModelResFactory& ModelResFactory::ModelResFactory::GetInstance()
{
	static ModelResFactory fac;
	return fac;
}

std::vector<std::shared_ptr<BindableInterface>>& ModelResFactory::GetResource(const std::string& model_key)
{
	std::map<std::string, std::vector<std::shared_ptr<BindableInterface>>>::iterator it;
	if (!Exist(model_key, it))
		return std::vector<std::shared_ptr<BindableInterface>>{};
	else
		return it->second;
}


void ModelResFactory::AddResource(std::string model_key)
{
	if (Exist(model_key)) return;
	std::vector<std::shared_ptr<BindableInterface>> v;
	std::vector<Postion3DTN>** pv = new std::vector<Postion3DTN>*;
	std::vector<UINT>** pi = new std::vector<UINT>*;
	MeshFactory::getInstance().GetMesh(model_key, pv, pi);
	v.push_back(std::make_shared<VertexBuffer<Postion3DTN, Vec>>(**pv, *gfx_));
	auto vs = std::make_shared<VertexShader>(*gfx_, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/VertexShader.cso");
	v.push_back(std::make_shared<IndexBuffer>(**pi, *gfx_));
	if (model_key == "point_light.obj" || model_key =="directional_light.obj" || model_key == "spot_light.obj")
	{
		v.push_back(std::make_shared<PixelShader>(*gfx_, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/p_raw_color.cso"));
		v.push_back(std::make_shared<PSConstantBuffer<LightShader>>(*gfx_, &gfx_->p_light_shader_));
	}
	else
	{
		v.push_back(std::make_shared<PixelShader>(*gfx_, "Y:/Project_VS2019/DX11RenderEngine/Shaders/cso/Lambert.cso"));
		v.push_back(std::make_shared<PSConstantBuffer<LightSet>>(*gfx_, &gfx_->p_scene_light_));
	}
	VertexLayout vl;
	//TODO():顶点布局要根据导入的obj文件进行调整
	vl << EVertexType::kPosition3D << EVertexType::kTexture2D << EVertexType::kNormal;
	v.push_back(std::make_shared<InputLayout>(*gfx_, *dynamic_cast<VertexShader*>(vs.get()), vl, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	v.push_back(vs);
	res_pool_.insert(std::pair<std::string, std::vector<std::shared_ptr<BindableInterface>>>(model_key, v));
	delete pv;
	delete pi;
}




