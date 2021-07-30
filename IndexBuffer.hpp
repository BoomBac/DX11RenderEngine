#pragma once
#include "Bindable.h"

template<typename T, template<typename U> typename Container>
class IndexBuffer : public Bindable
{
public:
	IndexBuffer(const Container<T>& vertics, ID3D11Device* pd);
	virtual void Bind(Graphics& gfx) override;

	void tBind(ID3D11DeviceContext& pc);
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer = nullptr;
};

template<typename T, template<typename U> typename Container>
IndexBuffer<T, Container>::IndexBuffer(const Container<T>& indices, ID3D11Device* pd)
{
	D3D11_BUFFER_DESC Ibd = {};
	Ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	Ibd.Usage = D3D11_USAGE_DEFAULT;
	Ibd.CPUAccessFlags = 0;
	Ibd.MiscFlags = 0;
	Ibd.ByteWidth = sizeof(indices);
	Ibd.StructureByteStride = sizeof(T);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = &indices.at(0);
	pd->CreateBuffer(&Ibd, &isd, pIndexBuffer.GetAddressOf());
}

template<typename T, template<typename U> typename Container>
void IndexBuffer<T, Container>::Bind(Graphics& gfx)
{
}

template<typename T, template<typename U> typename Container>
void IndexBuffer<T, Container>::tBind(ID3D11DeviceContext& pc)
{
	pc.IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
}
