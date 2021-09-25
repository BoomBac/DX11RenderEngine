#include "Public\Render\GeometryFactory.h"
#include "Public\Render\Shape\Box.h"
#include "Public\Render\Shape\Plane.h"

Graphics* GeometryFactory::gfx = nullptr;

GeometryFactory::GeometryFactory(Graphics* g)
{
	gfx = g;
}

void GeometryFactory::GenerateGeometry(EGeometryType g_type)
{
	assert(gfx != nullptr);
	Drawable* new_object = nullptr;
	static int box_count = 0;
	static int plane_count = 0;
	static int custom_count = 0;
	char object_name[64];
	switch (g_type)
	{
	case EGeometryType::kBox:
	{
		new_object = new Box(2,*gfx);
		sprintf(object_name, "Box_%d", box_count++);
	}
		break;
	case EGeometryType::kPlane:
	{
		new_object = new Plane(2, 2, 50, *gfx);
		sprintf(object_name, "Plane_%d", plane_count++);
	}
		break;
	case EGeometryType::kCustom:
		break;
	default:
		break;
	}
	if(new_object != nullptr)
	gfx->AddSceneObject(new_object, object_name);
}


