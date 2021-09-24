#include <d3dcompiler.h>

#include <Public/Render/Bindable/VertexShader.h>
#include <Public/Tool/Utili.h>

VertexShader::VertexShader(Graphics& gfx, const std::string& path)
{
	// 编译创建顶点着色器
	D3DReadFileToBlob(ToWide(path.c_str()).c_str(), &p_blob_);
	GetDevice(gfx)->CreateVertexShader(p_blob_->GetBufferPointer(), p_blob_->GetBufferSize(), nullptr,
		p_vertx_shader_.GetAddressOf());
}

void VertexShader::Bind(Graphics& gfx)
{
	GetContext(gfx)->VSSetShader(p_vertx_shader_.Get(), nullptr, 0);
}

EBindableType VertexShader::GetType() const
{
	return EBindableType::kVertexShader;
}
