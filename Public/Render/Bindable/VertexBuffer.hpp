#ifndef DX11ENGINE_RENDER_BINDABLE_VERTEXBUFFER_H
#define DX11ENGINE_RENDER_BINDABLE_VERTEXBUFFER_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

#include "BindableInterface.h"


template<typename T, template<typename U> typename Container>
class VertexBuffer : public BindableInterface
{
public:
	VertexBuffer(const Container<T>& vertics, Graphics& gfx);
	virtual void Bind(Graphics& gfx) override;
	virtual EBindableType GetType() const override;

private:
	DISALLOW_COPY_AND_ASSIGN(VertexBuffer)
	Microsoft::WRL::ComPtr<ID3D11Buffer> p_vertex_buffer_;
};

template<typename T, template<typename U> typename Container>
EBindableType VertexBuffer<T,Container>::GetType() const
{
	return EBindableType::kVertexBuffer;
}

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
	GetDevice(gfx)->CreateBuffer(&bd, &InitData, p_vertex_buffer_.GetAddressOf());
}

template<typename T, template<typename U> typename Container>
void VertexBuffer<T, Container>::Bind(Graphics& gfx)
{
	UINT stride = sizeof(T);
	UINT offset = 0u;
	GetContext(gfx)->IASetVertexBuffers(0u, 1u, p_vertex_buffer_.GetAddressOf(), &stride, &offset);
}

#endif //DX11ENGINE_RENDER_BINDABLE_VERTEXBUFFER_H