#ifndef DX11ENGINE_RENDER_SHAPE_PLANE_H
#define DX11ENGINE_RENDER_SHAPE_PLANE_H


#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

#include "vector2D.h"

#include "Public\Render\Drawable\Drawable.h"
#include "Public\Render\Shape\Shape.hpp"
#include "..\Texture.h"


class Plane :
	public Shape<Plane>
{
public:
	Plane() {};
	Plane(int row, int col, const int& size, Graphics& gfx);
	virtual void Draw(Graphics& gfx);
	//TODO(): add copy()
private:
	DISALLOW_COPY_AND_ASSIGN(Plane)
	std::vector<std::shared_ptr<Texture>> textures_;
};


#endif //DX11ENGINE_RENDER_SHAPE_PLANE_H