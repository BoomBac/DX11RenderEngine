#ifndef DX11ENGINE_RENDER_DRAWABLE_COORDINATE_H
#define DX11ENGINE_RENDER_DRAWABLE_COORDINATE_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif


#include "vector3D.h"

#include "Drawable.h"
#include "Public\Render\Shape\Shape.hpp"


class Coordinate :
	public Shape<Coordinate>
{
public:
	Coordinate() {};
	Coordinate(Graphics& gfx,const float& size);
	virtual void Draw(Graphics& gfx) override;
	//TODO(): add copy()
	void SetCoordinateType(bool is_world);
	bool GetCoordinateType() const;
private:
	DISALLOW_COPY_AND_ASSIGN(Coordinate)
	Drawable* object_attached_;
	bool is_world_;
};

#endif //DX11ENGINE_RENDER_DRAWABLE_COORDINATE_H
