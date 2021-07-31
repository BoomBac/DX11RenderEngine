#include "VertexShader.h"
#include <d3dcompiler.h>

namespace VShader
{
	std::wstring ToWide(const std::string& narrow)
	{
		wchar_t wide[512];
		mbstowcs_s(nullptr, wide, narrow.c_str(), _TRUNCATE);
		return wide;
	}
};



VertexShader::VertexShader(ID3D11Device* pd, const std::string& path)
{
	// 编译创建顶点着色器
	D3DReadFileToBlob(VShader::ToWide(path.c_str()).c_str(), &pVSBlob);
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
