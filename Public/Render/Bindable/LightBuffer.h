#ifndef DX11ENGINE_RENDER_BINDABLE_LIGHTBUFFER_H
#define DX11ENGINE_RENDER_BINDABLE_LIGHTBUFFER_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

#include <DirectXMath.h>

#include "BindableInterface.h"
#include "ConstantBuffer.h"
#include "Public/Render/Light/Light.h"


template<class T>
class LightBuffer :
	public BindableInterface
{
public:
	LightBuffer(Graphics& gfx, Light& object);
	virtual void Bind(Graphics& gfx) override;
	virtual EBindableType GetType() const override;
private:

	DISALLOW_COPY_AND_ASSIGN(LightBuffer)
	PConstantBuffer<T> p_const_buf_;
	Light& object_;
};

template<class T>
LightBuffer<T>::LightBuffer(Graphics& gfx, Light& object) :p_const_buf_(gfx), object_(object)
{
}
template<class T>
void LightBuffer<T>::Bind(Graphics& gfx)
{
	p_const_buf_.Update(gfx, object_.GetLightAttribute());
	p_const_buf_.Bind(gfx);
}
template<class T>
EBindableType LightBuffer<T>::GetType() const
{
	return EBindableType::kConstantBuffer;
}

#endif //DX11ENGINE_RENDER_BINDABLE_LIGHTBUFFER_H