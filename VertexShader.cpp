#include "VertexShader.h"
#include <d3dcompiler.h>
#include "Utili.h"



VertexShader::VertexShader(ID3D11Device* pd, const std::string& path)
{
	// 编译创建顶点着色器
	D3DReadFileToBlob(Utility::ToWide(path.c_str()).c_str(), &pVSBlob);
	pd->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr,
		pVShader.GetAddressOf());
}

void VertexShader::Bind(Graphics& gfx)
{
}

void VertexShader::TBind(ID3D11DeviceContext& pc)
{
	pc.VSSetShader(pVShader.Get(), nullptr, 0);
}
