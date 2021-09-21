#include <d3dcompiler.h>

#include <Public/Render/Bindable/VertexShader.h>
#include <Public/Tool/Utili.h>

VertexShader::VertexShader(Graphics& gfx, const std::string& path)
{
	// 编译创建顶点着色器
	D3DReadFileToBlob(ToWide(path.c_str()).c_str(), &pVSBlob);
	GetDevice(gfx)->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr,
		pVShader.GetAddressOf());
}

void VertexShader::Bind(Graphics& gfx)
{
	GetContext(gfx)->VSSetShader(pVShader.Get(), nullptr, 0);
}

EBindableType VertexShader::GetType() const
{
	return EBindableType::VertexShader;
}
