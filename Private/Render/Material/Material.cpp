#include "Public/Render/Material/Material.h"
#include "Public/Render/ResManage/PShaderFactory.h"
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <QDebug>
#include <Public/Global.h>

using Microsoft::WRL::ComPtr;


void Material::LoadFromLib(const std::string& name)
{
	p_shader_ = PShaderFactory::getInstance().GetShader(name);
	ComPtr<ID3D11ShaderReflection> shader_reflection;
	HRESULT hr = D3DReflect(p_shader_->p_blob_->GetBufferPointer(), p_shader_->p_blob_->GetBufferSize(),
		IID_ID3D11ShaderReflection, reinterpret_cast<void**>(shader_reflection.GetAddressOf()));
	for (UINT i = 0;; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC des;
		hr = shader_reflection->GetResourceBindingDesc(i, &des);
		if (FAILED(hr))
		{
			qDebug() << "Input Resource Parse Done";
			break;
		}
		UINT cbuf_slot = 0;
		switch (des.Type)
		{
		case D3D_SIT_CBUFFER:
		{
			ID3D11ShaderReflectionConstantBuffer* pRcbuf = shader_reflection->GetConstantBufferByName(des.Name);
			D3D11_SHADER_BUFFER_DESC cbDesc{};
			hr = pRcbuf->GetDesc(&cbDesc);
			if (FAILED(hr))
			{
				qDebug() << "Parse ConstBuffer Failed";
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
			Global::getInstance()->G_Device->CreateBuffer(&cdb, nullptr, g_buf.GetAddressOf());
			p_cbuffer_.push_back(p);
			p_gbuffer_.push_back(g_buf);
			buf_size_.push_back(buf_size);
			++cbuf_slot;
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
	Global::getInstance()->G_Context->PSSetConstantBuffers(1u,p_gbuffer_.size(), p_gbuffer_.data()->GetAddressOf());
	p_shader_->Bind(*Global::getInstance()->G_Gfx);
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
	//qDebug() << "peek:" << *reinterpret_cast<float*>(p_cbuffer_[0]);
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

void Material::CommitAllBufferData()
{
	D3D11_MAPPED_SUBRESOURCE mapSub;
	//qDebug() << "value in cbuf" << *reinterpret_cast<float*>(p_cbuffer_[0]);
	//SetFloat("light_intensity", 0.2f);
	for (int i = 0; i < p_cbuffer_.size(); ++i)
	{

		auto hr = Global::getInstance()->G_Context->Map(p_gbuffer_[i].Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapSub);
		memcpy(mapSub.pData, p_cbuffer_[i], buf_size_[i]);
		//memcpy(mapSub.pData, &j, 4);
		//qDebug() << *reinterpret_cast<float*>(mapSub.pData);
		Global::getInstance()->G_Context->Unmap(p_gbuffer_[i].Get(), 0u);
	}
	//qDebug() << "value in gbuf" << *reinterpret_cast<float*>(mapSub.pData);
	Bind();
}
