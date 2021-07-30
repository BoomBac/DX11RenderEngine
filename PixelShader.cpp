#include "PixelShader.h"
#include <d3dcompiler.h>
#include "Utili.h"

PixelShader::PixelShader(ID3D11Device* pd, const std::string& path)
{
	D3DReadFileToBlob(Utility::ToWide(path).c_str(), &pVSBlob);
	pd->CreatePixelShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pPShader);
}

void PixelShader::Bind(Graphics& gfx)
{
}

void PixelShader::TBind(ID3D11DeviceContext& pc)
{
	pc.PSSetShader(pPShader.Get(), nullptr, 0);
}
