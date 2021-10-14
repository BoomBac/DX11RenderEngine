#ifndef DX11ENGINE_RENDER_BINDABLE_LIGHTBUFFER_H
#define DX11ENGINE_RENDER_BINDABLE_LIGHTBUFFER_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif



#include "BindableInterface.h"
#include "ConstantBuffer.h"



template<class T>
class PSConstantBuffer :
	public BindableInterface
{
public:
	PSConstantBuffer(Graphics& gfx, T** buf);
	virtual void Bind(Graphics& gfx) override;
	virtual EBindableType GetType() const override;
private:

	DISALLOW_COPY_AND_ASSIGN(PSConstantBuffer)
	PConstantBuffer<T> p_const_buf_;
	T** buf_;
};

template<class T>
PSConstantBuffer<T>::PSConstantBuffer(Graphics& gfx, T** buf) :p_const_buf_(gfx),buf_(buf)
{
}
template<class T>
void PSConstantBuffer<T>::Bind(Graphics& gfx)
{
	p_const_buf_.Update(gfx, **buf_);
	dynamic_cast<BindableInterface*>(&p_const_buf_)->pc_buf_index = dynamic_cast<BindableInterface*>(this)->pc_buf_index;
	p_const_buf_.Bind(gfx);
}
template<class T>
EBindableType PSConstantBuffer<T>::GetType() const
{
	return EBindableType::kPixelConstantBuffer;
}

#endif //DX11ENGINE_RENDER_BINDABLE_LIGHTBUFFER_H