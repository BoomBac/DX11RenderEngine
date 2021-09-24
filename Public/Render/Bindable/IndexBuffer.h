#ifndef DX11ENGINE_RENDER_BINDABLE_INDEXBUFFER_H
#define DX11ENGINE_RENDER_BINDABLE_INDEXBUFFER_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif


#include "BindableInterface.h"


class IndexBuffer : public BindableInterface
{
public:
	IndexBuffer(const std::vector<UINT>& indices, Graphics& gfx);
	virtual void Bind(Graphics& gfx) override;
	virtual EBindableType GetType() const override;

	int size_;
private:
	DISALLOW_COPY_AND_ASSIGN(IndexBuffer)
	Microsoft::WRL::ComPtr<ID3D11Buffer> p_index_buffe_r = nullptr;

};

#endif //DX11ENGINE_RENDER_BINDABLE_INDEXBUFFER_H