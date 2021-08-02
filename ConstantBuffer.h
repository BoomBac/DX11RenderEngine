#pragma once
#include "Bindable.h"
template<class T>
class ConstantBuffer : public Bindable
{
public:
	ConstantBuffer(Graphics& gfx,const T& buffer);
	ConstantBuffer(Graphics& gfx);
	void Update(Graphics& gfx,const T&buffer);
	EBindableType GetType() const override
	{
		return EBindableType::ConstantBuffer;
	}
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
};

template<class T>
ConstantBuffer<T>::ConstantBuffer(Graphics& gfx)
{
	D3D11_BUFFER_DESC cdb;
	cdb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cdb.Usage = D3D11_USAGE_DYNAMIC;
	cdb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cdb.MiscFlags = 0;
	cdb.ByteWidth = sizeof(T);
	cdb.StructureByteStride = 0;
	GetDevice(gfx)->CreateBuffer(&cdb, nullptr, pConstantBuffer.GetAddressOf());
}

template<class T>
void ConstantBuffer<T>::Update(Graphics& gfx, const T& buffer)
{
	D3D11_MAPPED_SUBRESOURCE mapSub;
	GetContext(gfx)->Map(pConstantBuffer.Get(), 0u
		, D3D11_MAP_WRITE_DISCARD, 0u, &mapSub);
	memcpy(mapSub.pData, &buffer, sizeof(buffer)); // destination source sizeof(source)
	GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
}

template<class T>
ConstantBuffer<T>::ConstantBuffer(Graphics& gfx, const T& buffer)
{
	D3D11_BUFFER_DESC cdb;
	cdb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cdb.Usage = D3D11_USAGE_DYNAMIC;
	cdb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cdb.MiscFlags = 0;
	cdb.ByteWidth = sizeof(buffer);
	cdb.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &buffer;
	GetDevice(gfx)->CreateBuffer(&cdb, &csd, pConstantBuffer.GetAddressOf());
}



template<class T>
class VConstantBuffer : public ConstantBuffer<T>
{
public:
	using ConstantBuffer<T>::ConstantBuffer;
	virtual void Bind(Graphics& gfx) override;
	void TBind(ID3D11DeviceContext& dc);
};
template<class T>
void VConstantBuffer<T>::Bind(Graphics& gfx)
{
	GetContext(gfx)->VSSetConstantBuffers(0, 1, pConstantBuffer.GetAddressOf());
}

template<class T>
class PConstantBuffer : public ConstantBuffer<T>
{
public:
	using ConstantBuffer<T>::ConstantBuffer;
	virtual void Bind(Graphics& gfx) override;
	void TBind(ID3D11DeviceContext& dc);
};
template<class T>
void PConstantBuffer<T>::Bind(Graphics& gfx)
{
	GetContext(gfx)->PSSetConstantBuffers(0, 1, pConstantBuffer.GetAddressOf());
}
