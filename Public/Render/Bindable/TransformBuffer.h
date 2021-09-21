#pragma once
#include "Bindable.h"
#include "ConstantBuffer.h"
#include <DirectXMath.h>
#include <Public/Render/Drawable/Drawable.h>
class TransformBuffer :
    public Bindable
{
public:
    TransformBuffer(Graphics& gfx,Drawable& object);
    virtual void Bind(Graphics& gfx) override;
    virtual EBindableType GetType() const override;
private:
    VConstantBuffer<WorldTransform> vbuf;
    Drawable& object;
};

