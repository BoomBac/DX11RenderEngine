#ifndef DX11ENGINE_RENDER_BINDABLE_TRANSFORMBUFFER_H
#define DX11ENGINE_RENDER_BINDABLE_TRANSFORMBUFFER_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

#include <DirectXMath.h>

#include "BindableInterface.h"
#include "ConstantBuffer.h"
#include <Public/Render/Drawable/Drawable.h>

class TransformBuffer :
    public BindableInterface
{
public:
    TransformBuffer(Graphics& gfx,Drawable& object);
    virtual void Bind(Graphics& gfx) override;
    virtual EBindableType GetType() const override;
private:

	DISALLOW_COPY_AND_ASSIGN(TransformBuffer)
    VConstantBuffer<WorldTransform> v_const_buf_;
    Drawable& object_;
};

#endif //DX11ENGINE_RENDER_BINDABLE_TRANSFORMBUFFER_H
