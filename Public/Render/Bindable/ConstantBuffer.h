#ifndef DX11ENGINE_RENDER_BINDABLE_CONSTANTBUFFER_H
#define DX11ENGINE_RENDER_BINDABLE_CONSTANTBUFFER_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif


#include "BindableInterface.h"

template<class T>
class ConstantBuffer : public BindableInterface
{
public:
	ConstantBuffer(Graphics& gfx,const T& buffer);
	ConstantBuffer(Graphics& gfx);
	void Update(Graphics& gfx,const T&buffer);
	EBindableType GetType() const override
	{
		return EBindableType::kConstantBuffer;
	}
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> p_constant_buffer_;
private:
	DISALLOW_COPY_AND_ASSIGN(ConstantBuffer)
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
	GetDevice(gfx)->CreateBuffer(&cdb, nullptr, p_constant_buffer_.GetAddressOf());
}

template<class T>
void ConstantBuffer<T>::Update(Graphics& gfx, const T& buffer)
{
	D3D11_MAPPED_SUBRESOURCE mapSub;
	GetContext(gfx)->Map(p_constant_buffer_.Get(), 0u
		, D3D11_MAP_WRITE_DISCARD, 0u, &mapSub);
	memcpy(mapSub.pData, &buffer, sizeof(buffer)); // destination source sizeof(source)
	GetContext(gfx)->Unmap(p_constant_buffer_.Get(), 0u);
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
	GetDevice(gfx)->CreateBuffer(&cdb, &csd, p_constant_buffer_.GetAddressOf());
}



template<class T>
class VConstantBuffer : public ConstantBuffer<T>
{
public:
	using ConstantBuffer<T>::ConstantBuffer;
	virtual void Bind(Graphics& gfx) override;
	EBindableType GetType() const override
	{
		return EBindableType::kVetexConstantBuffer;
	}
private:
	DISALLOW_COPY_AND_ASSIGN(VConstantBuffer)
};
template<class T>
void VConstantBuffer<T>::Bind(Graphics& gfx)
{
	//GetContext(gfx)->VSSetConstantBuffers(1, 1, p_constant_buffer_.GetAddressOf());
	GetContext(gfx)->VSSetConstantBuffers(BindableInterface::vc_buf_index_, 1, p_constant_buffer_.GetAddressOf());
}

template<class T>
class PConstantBuffer : public ConstantBuffer<T>
{
public:
	using ConstantBuffer<T>::ConstantBuffer;
	virtual void Bind(Graphics& gfx) override;
	EBindableType GetType() const override
	{
		return EBindableType::kPixelConstantBuffer;
	}
private:
	DISALLOW_COPY_AND_ASSIGN(PConstantBuffer)
};
template<class T>
void PConstantBuffer<T>::Bind(Graphics& gfx)
{
	GetContext(gfx)->PSSetConstantBuffers(BindableInterface::pc_buf_index, 1, p_constant_buffer_.GetAddressOf());
}

#endif //DX11ENGINE_RENDER_BINDABLE_CONSTANTBUFFER_H