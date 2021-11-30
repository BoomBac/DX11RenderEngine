#include "Public/Render/Material/Material.h"
#include "Public/Render/ResManage/ShaderFactory.h"
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <QDebug>
#include <Public/Global.h>
#include <Public/Render/ResManage/TextureFactory.h>

using Microsoft::WRL::ComPtr;


void Material::LoadFromLib(const std::string& name)
{
	p_shader_ = ShaderFactory::getInstance().GetPShader(name);
	ComPtr<ID3D11ShaderReflection> shader_reflection;
	HRESULT hr = D3DReflect(p_shader_->p_blob_->GetBufferPointer(), p_shader_->p_blob_->GetBufferSize(),
		IID_ID3D11ShaderReflection, reinterpret_cast<void**>(shader_reflection.GetAddressOf()));
	UINT cbuf_slot = 0;
	p_cbuffer_.fill(nullptr);
	buf_size_.fill(0);
	for (UINT i = 0;; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC des;
		hr = shader_reflection->GetResourceBindingDesc(i, &des);
		if (FAILED(hr))
		{
			qDebug() << "Input Resource Parse Done";
			break;
		}

		switch (des.Type)
		{
		case D3D_SIT_CBUFFER:
		{
			ID3D11ShaderReflectionConstantBuffer* pRcbuf = shader_reflection->GetConstantBufferByIndex(cbuf_slot);
			D3D11_SHADER_BUFFER_DESC cbDesc{};
			hr = pRcbuf->GetDesc(&cbDesc);
			if (FAILED(hr))
			{
				qDebug() << "Parse ConstBuffer Failed";
				++cbuf_slot;
				break;
			}
			UINT buf_size = 0;
			std::vector<CBufferValueInfo> buf_info;
			for (UINT j = 0; j < cbDesc.Variables; ++j)
			{
				ID3D11ShaderReflectionVariable* pRval = pRcbuf->GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC vDes{};
				hr = pRval->GetDesc(&vDes);
				if (FAILED(hr))
				{
					qDebug() << "Parse Vaule Failed";
					break;
				}
				cval_info_.insert(std::pair<std::string, CBufferValueInfo>(vDes.Name, { cbuf_slot,vDes.StartOffset }));
				buf_size += vDes.Size;
			}
			void*p = ::operator new(buf_size);
			D3D11_BUFFER_DESC cdb;
			cdb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cdb.Usage = D3D11_USAGE_DYNAMIC;
			cdb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cdb.MiscFlags = 0;
			cdb.ByteWidth = buf_size;
			cdb.StructureByteStride = 0;
			ComPtr<ID3D11Buffer> g_buf;
			hr = Global::getInstance()->G_Device->CreateBuffer(&cdb, nullptr, g_buf.GetAddressOf());
			p_cbuffer_[cbuf_slot] = p;
			p_gbuffer_[cbuf_slot] = g_buf;
			buf_size_[cbuf_slot] = buf_size;
			++cbuf_slot;
			++buffer_num;
		}
		break;
		case D3D_SIT_TEXTURE:
		{
			texture_struct_.insert(std::move(std::pair<std::string, UINT>(des.Name, des.BindPoint)));
		}
		break;
		case D3D_SIT_SAMPLER:
		{
			sampler_struct_.insert(std::move(std::pair<std::string, UINT>(des.Name, des.BindPoint)));
		}
		default:
			break;
		}

	}
	qDebug() << "hello";
}

void Material::Bind()
{
	Global::getInstance()->G_Context->PSSetConstantBuffers(0, buffer_num, p_gbuffer_.data()->GetAddressOf());
	//p_shader_->Bind(*Global::getInstance()->G_Gfx);
}

Material::~Material()
{
	if (p_cbuffer_.size() > 0)
		for (auto& buf : p_cbuffer_)
		::operator delete(buf);
}

bool Material::SetFloat(const std::string& name, const float& value)
{
	auto it = cval_info_.find(name);
	if ( it == cval_info_.end())
	{
		qDebug() << "Parameters do not exist";
		return false;
	}
	memcpy(reinterpret_cast<char*>(p_cbuffer_[it->second.slot]) + it->second.offset,
		&value, sizeof(value));
	//qDebug() << *reinterpret_cast<float*>(p_cbuffer_[it->second.slot]);
	return true;
}

bool Material::SetFloat3(const std::string& name, const DirectX::XMFLOAT3& value)
{
	auto it = cval_info_.find(name);
	if (it == cval_info_.end())
	{
		qDebug() << "Parameters do not exist";
		return false;
	}
	memcpy(reinterpret_cast<DirectX::XMFLOAT3*>(reinterpret_cast<char*>(p_cbuffer_[it->second.slot]) + it->second.offset),
		&value, sizeof(value));
	return true;
}

bool Material::SetFloat4(const std::string& name, const DirectX::XMFLOAT4& value)
{
	auto it = cval_info_.find(name);
	if (it == cval_info_.end())
	{
		qDebug() << "Parameters do not exist";
		return false;
	}
	memcpy(reinterpret_cast<DirectX::XMFLOAT4*>(reinterpret_cast<char*>(p_cbuffer_[it->second.slot]) + it->second.offset),
		&value, sizeof(value));
	return true;
}

bool Material::SetMatrix4x4(const std::string& name, const DirectX::XMMATRIX& value)
{
	auto it = cval_info_.find(name);
	if (it == cval_info_.end())
	{
		qDebug() << "Parameters do not exist";
		return false;
	}
	memcpy(reinterpret_cast<DirectX::XMMATRIX*>(reinterpret_cast<char*>(p_cbuffer_[it->second.slot]) + it->second.offset),
		&value, sizeof(value));
	return true;
}

bool Material::SetTextureByIndex(const UINT& id, const std::string& tex_name)
{
	return false;
}

bool Material::SetTextureByName(const std::string& name, const std::string& tex_name)
{
	return false;
}

void Material::InitTexture(const std::string& name, ESResourceType tex_type)
{
	texture_list_.insert(std::make_pair(tex_type, name));
	textures_[tex_type] = TextureFactory::GetInstance().GetTexture(name);

}

void Material::InitTexture(const std::vector<std::string>& name_list)
{
	UINT i = 0;
	for (auto& name : name_list)
	{
		texture_list_.insert(std::pair<ESResourceType,std::string>(static_cast<ESResourceType>(i), name));
		textures_[i] = TextureFactory::GetInstance().GetTexture(name);
		++i;
	}
	texture_num_ = i;
}

void Material::CommitAllBufferData()
{
	D3D11_MAPPED_SUBRESOURCE mapSub;
	for (int i = 0; i < buffer_num; ++i)
	{
		if (p_cbuffer_[i] == nullptr) continue;
		auto hr = Global::getInstance()->G_Context->Map(p_gbuffer_[i].Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapSub);
		memcpy(mapSub.pData, p_cbuffer_[i], buf_size_[i]);
		Global::getInstance()->G_Context->Unmap(p_gbuffer_[i].Get(), 0u);
	}
	Bind();
}

void Material::CommitAllTexture()
{
	ID3D11ShaderResourceView* tex[5];
	for (int i = 0; i < texture_num_; ++i)
	{
		tex[i] = textures_[i]->GetTextureResourceView();
	}
	Global::getInstance()->G_Context->PSSetShaderResources(0, texture_num_, tex);
	Bind();
}

void Material::CommitAllInput()
{
	CommitAllBufferData();
	CommitAllTexture();
	Bind();
}
