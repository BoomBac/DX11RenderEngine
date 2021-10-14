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
	DirectX::CreateWICTextureFromFile(device, ToWide(file_path).c_str(), nullptr, p_textureView_.GetAddressOf());
}

void Texture::Load(const char* file_path)
{
	DirectX::CreateWICTextureFromFile(device_, ToWide(file_path).c_str(), nullptr, p_textureView_.GetAddressOf());
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


