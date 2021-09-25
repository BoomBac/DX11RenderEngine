#ifndef DX11ENGINE_RENDER_SHAPE_BOX_H
#define DX11ENGINE_RENDER_SHAPE_BOX_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif


#include "vector3D.h"

#include "Public\Render\Drawable\Drawable.h"
#include "Public\Render\Shape\Shape.hpp"


class Box :
    public Shape<Box>
{
public:
    Box() {};
    Box(const int& size, Graphics& gfx);
    //TODO(): add copy()
private:
    DISALLOW_COPY_AND_ASSIGN(Box)
};

#endif //DX11ENGINE_RENDER_SHAPE_BOX_H
