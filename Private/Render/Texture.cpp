#include "WICTextureLoader.h"

#include "Public/Render/Texture.h"
#include "Public/Tool/Utili.h"

#pragma comment(lib,"DirectXTK.lib")

ID3D11Device* Texture::device_ = nullptr;

Texture::Texture(ID3D11Device* device)
{
	device_ = device;
}

Texture::Texture(ID3D11Device* device, const char* file_path)
{
	device_ = device;
	DirectX::CreateWICTextureFromFile(device, ToWide(file_path).c_str(), p_texture_.GetAddressOf(), p_textureView_.GetAddressOf());
}

Texture::Texture(ID3D11Device* device, const char* file_path, const D3D11_TEXTURE2D_DESC& des)
{
	device_ = device;
	DirectX::CreateWICTextureFromFileEx(device_, ToWide(file_path).c_str(),0,
		des.Usage,des.BindFlags,des.CPUAccessFlags,des.MiscFlags,WIC_LOADER_FLAGS::WIC_LOADER_DEFAULT,p_texture_.GetAddressOf()
	,p_textureView_.GetAddressOf());
}

void Texture::Load(const char* file_path)
{
	DirectX::CreateWICTextureFromFile(device_, ToWide(file_path).c_str(), p_texture_.GetAddressOf(), p_textureView_.GetAddressOf());
}

ID3D11ShaderResourceView* Texture::GetTextureResourceView()
{
	if (p_textureView_!=nullptr)
	{
		return p_textureView_.Get();
	}
}

ID3D11ShaderResourceView** Texture::GetTextureResourceViewAddress()
{
	if (p_textureView_ != nullptr)
	{
		return p_textureView_.GetAddressOf();
	}
}

ID3D11Resource* Texture::GetResource()
{
	return p_texture_.Get();
}


