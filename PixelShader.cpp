#include "PixelShader.h"
#include <d3dcompiler.h>
#include "Utili.h"

PixelShader::PixelShader(Graphics& gfx, const std::string& path)
{
	D3DReadFileToBlob(ToWide(path).c_str(), &pVSBlob);
	GetDevice(gfx)->CreatePixelShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pPShader);
}

void PixelShader::Bind(Graphics& gfx)
{
	GetContext(gfx)->PSSetShader(pPShader.Get(), nullptr, 0);
}

void PixelShader::TBind(ID3D11DeviceContext& pc)
{
	pc.PSSetShader(pPShader.Get(), nullptr, 0);
}
