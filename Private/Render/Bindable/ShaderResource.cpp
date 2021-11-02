#include "Public/Render/Bindable/ShaderResource.h"

ShaderResource::ShaderResource(ID3D11ShaderResourceView* srv, ESResourceType type) : p_shader_res_v_(srv),tex_type_(type)
{

}

void ShaderResource::Bind(Graphics& gfx)
{
	switch (tex_type_)
	{
	case ESResourceType::kDiffuse:
	{
		gfx.GetContext()->PSSetShaderResources(0u, 1u, &p_shader_res_v_);
	}
		break;
	case ESResourceType::kNormal:
	{
		gfx.GetContext()->PSSetShaderResources(4u, 1u, &p_shader_res_v_);
	}
		break;
	case ESResourceType::kMetallic:
	{
		gfx.GetContext()->PSSetShaderResources(1u, 1u, &p_shader_res_v_);
	}
		break;
	case ESResourceType::kAlbedo:
	{
		gfx.GetContext()->PSSetShaderResources(3u, 1u, &p_shader_res_v_);
	}
		break;
	case ESResourceType::kRoughness:
	{
		gfx.GetContext()->PSSetShaderResources(2u, 1u, &p_shader_res_v_);
	}
		break;
	case ESResourceType::kAmbientOcclusion:
	{
		gfx.GetContext()->PSSetShaderResources(5u, 1u, &p_shader_res_v_);
	}
		break;
	case ESResourceType::kIrradiance:
	{
		gfx.GetContext()->PSSetShaderResources(6u, 1u, &p_shader_res_v_);
	}
	break;
	case ESResourceType::kSpecularMap:
	{
		gfx.GetContext()->PSSetShaderResources(7u, 1u, &p_shader_res_v_);
	}
	break;
	case ESResourceType::kLUT:
	{
		gfx.GetContext()->PSSetShaderResources(8u, 1u, &p_shader_res_v_);
	}
	default:
		break;
	}

}

void ShaderResource::SetTexture(ID3D11ShaderResourceView* srv, ESResourceType type)
{
	p_shader_res_v_ = srv;
	tex_type_ = type;
}

ID3D11ShaderResourceView* ShaderResource::GetResourceView()
{
	return p_shader_res_v_;
}

ESResourceType ShaderResource::GetTextureType() const
{
	return tex_type_;
}

EBindableType ShaderResource::GetType() const
{
	return EBindableType::kShaderResource;
}
