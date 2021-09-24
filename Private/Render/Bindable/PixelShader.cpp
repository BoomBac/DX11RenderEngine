#include <d3dcompiler.h>

#include <Public/Render/Bindable/PixelShader.h>
#include <Public/Tool/Utili.h>


PixelShader::PixelShader(Graphics& gfx, const std::string& path)
{
	D3DReadFileToBlob(ToWide(path).c_str(), &p_blob_);
	GetDevice(gfx)->CreatePixelShader(p_blob_->GetBufferPointer(), p_blob_->GetBufferSize(), nullptr, &p_pixel_shader_);
}

void PixelShader::Bind(Graphics& gfx)
{
	GetContext(gfx)->PSSetShader(p_pixel_shader_.Get(), nullptr, 0);
}

EBindableType PixelShader::GetType() const
{
	return EBindableType::kPixelShader;
}

