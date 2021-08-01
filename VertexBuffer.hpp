#pragma once
#include "Bindable.h"


template<typename T, template<typename U> typename Container>
class VertexBuffer : public Bindable
{
public:
	VertexBuffer(const Container<T>& vertics, Graphics& gfx);
	
	virtual void Bind(Graphics& gfx) override;

	void tBind(ID3D11DeviceContext& pc);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};

template<typename T, template<typename U> typename Container>
VertexBuffer<T, Container>::VertexBuffer(const Container<T>& vertics, Graphics& gfx)
{
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.ByteWidth = sizeof(T)*vertics.size();
	bd.StructureByteStride = sizeof(T);
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &vertics.at(0);
	GetDevice(gfx)->CreateBuffer(&bd, &InitData, pVertexBuffer.GetAddressOf());
}

template<typename T, template<typename U> typename Container>
void VertexBuffer<T, Container>::Bind(Graphics& gfx)
{
	UINT stride = sizeof(T);
	UINT offset = 0u;
	GetContext(gfx)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
}

template<typename T, template<typename U> typename Container>
void VertexBuffer<T, Container>::tBind(ID3D11DeviceContext& pc)
{
	UINT stride = sizeof(T);
	UINT offset = 0u;
	pc.IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
}
