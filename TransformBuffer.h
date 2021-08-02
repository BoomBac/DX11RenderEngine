#pragma once
#include "Bindable.h"
#include "Drawable.h"
#include "ConstantBuffer.h"
#include <DirectXMath.h>
class TransformBuffer :
    public Bindable
{
public:
    TransformBuffer(Graphics& gfx,Drawable& object);
    virtual void Bind(Graphics& gfx) override;
private:
    VConstantBuffer<MVPTransform> vbuf;
    Drawable& object;
};

