#include "Public/Render/Bindable/ShaderResource.h"

ShaderResource::ShaderResource(ID3D11ShaderResourceView* srv, ETextureType type) : p_shader_res_v_(srv),tex_type_(type)
{

}

void ShaderResource::Bind(Graphics& gfx)
{
	switch (tex_type_)
	{
	case ETextureType::kDiffuse:
	{
		gfx.GetContext()->PSSetShaderResources(0u, 1u, &p_shader_res_v_);
	}
		break;
	case ETextureType::kNormal:
	{
		gfx.GetContext()->PSSetShaderResources(4u, 1u, &p_shader_res_v_);
	}
		break;
	case ETextureType::kMetallic:
	{
		gfx.GetContext()->PSSetShaderResources(1u, 1u, &p_shader_res_v_);
	}
		break;
	case ETextureType::kAlbedo:
	{
		gfx.GetContext()->PSSetShaderResources(3u, 1u, &p_shader_res_v_);
	}
		break;
	case ETextureType::kRoughness:
	{
		gfx.GetContext()->PSSetShaderResources(2u, 1u, &p_shader_res_v_);
	}
		break;
	case ETextureType::kAmbientOcclusion:
	{
		gfx.GetContext()->PSSetShaderResources(5u, 1u, &p_shader_res_v_);
	}
		break;
	case ETextureType::kIrradiance:
	{
		gfx.GetContext()->PSSetShaderResources(6u, 1u, &p_shader_res_v_);
	}
	default:
		break;
	}

}

void ShaderResource::SetTexture(ID3D11ShaderResourceView* srv, ETextureType type)
{
	p_shader_res_v_ = srv;
	tex_type_ = type;
}

ID3D11ShaderResourceView* ShaderResource::GetResourceView()
{
	return p_shader_res_v_;
}

ETextureType ShaderResource::GetTextureType() const
{
	return tex_type_;
}

EBindableType ShaderResource::GetType() const
{
	return EBindableType::kShaderResource;
}
