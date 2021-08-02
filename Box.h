#pragma once
#include "Drawable.h"
#include "vector3D.h"
class Box :
    public Drawable
{
public:
    Box() {};
    Box(const CusMath::vector3d& initPos, const int& size, Graphics& gfx);
    void Update(const DirectX::XMMATRIX& transf);
};

