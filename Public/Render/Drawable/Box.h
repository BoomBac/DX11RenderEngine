#pragma once
#include "Drawable.h"
#include "vector3D.h"
#include "Public\Render\Shape\Shape.hpp"


class Box :
    public Shape<Box>
{
public:
    Box() {};
    Box(const CusMath::vector3d& initPos, const int& size, Graphics& gfx);
};

