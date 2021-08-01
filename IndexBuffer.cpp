#include "IndexBuffer.h"
#include <minwinbase.h>


IndexBuffer::IndexBuffer(const std::vector<UINT>& indices, Graphics& gfx)
{
	D3D11_BUFFER_DESC Ibd = {};
	Ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	Ibd.Usage = D3D11_USAGE_DEFAULT;
	Ibd.CPUAccessFlags = 0;
	Ibd.MiscFlags = 0;
	Ibd.ByteWidth = sizeof(UINT)*indices.size();
	Ibd.StructureByteStride = sizeof(UINT);
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &indices.at(0);
	GetDevice(gfx)->CreateBuffer(&Ibd, &InitData, pIndexBuffer.GetAddressOf());
	count = indices.size();
}

void IndexBuffer::Bind(Graphics& gfx)
{
	GetContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}
