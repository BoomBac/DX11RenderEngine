#ifndef DX11ENGINE_RENDER_GEOMETRYFACTORY_H
#define DX11ENGINE_RENDER_GEOMETRYFACTORY_H
#include "Drawable/Drawable.h"
#include "Graphics.h"


#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

class GeometryFactory
{
public:
    GeometryFactory(Graphics* g);
    //根据类型生成内置几何体或者默认模型
    static Drawable* GenerateGeometry(EGeometryType g_type);
    //根据模型名称生成
    static Drawable* GenerateGeometry(const char* file_name);
private:
    DISALLOW_COPY_AND_ASSIGN(GeometryFactory)
    static Graphics* gfx;
};

#endif //DX11ENGINE_RENDER_GEOMETRYFACTORY_H