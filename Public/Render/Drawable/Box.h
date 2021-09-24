#ifndef DX11ENGINE_RENDER_DRAWABLE_BOX_H
#define DX11ENGINE_RENDER_DRAWABLE_BOX_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif


#include "vector3D.h"

#include "Drawable.h"
#include "Public\Render\Shape\Shape.hpp"


class Box :
    public Shape<Box>
{
public:
    Box() {};
    Box(const CusMath::vector3d& initPos, const int& size, Graphics& gfx);
    //TODO(): add copy()
private:
    DISALLOW_COPY_AND_ASSIGN(Box)
};

#endif //DX11ENGINE_RENDER_DRAWABLE_BOX_H
