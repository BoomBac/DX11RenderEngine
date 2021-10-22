#include "Public\Render\GeometryFactory.h"
#include "Public\Render\Shape\Box.h"
#include "Public\Render\Shape\Plane.h"
#include "Public\Render\Shape\Model.h"
#include "Public/Render/Shape/SkyBox.h"

Graphics* GeometryFactory::gfx = nullptr;

namespace
{
	static int model_count = 0;
}

GeometryFactory::GeometryFactory(Graphics* g)
{
	gfx = g;
}

Drawable* GeometryFactory::GenerateGeometry(EGeometryType g_type)
{
	assert(gfx != nullptr);
	Drawable* new_object = nullptr;
	static int box_count = 0;
	static int plane_count = 0;

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
	{
		new_object = new Model(*gfx);
		sprintf(object_name, "Model_%d", model_count++);
	}
	break;
	case EGeometryType::kSkyBox:
	{
		new_object = new SkyBox(*gfx);
		sprintf(object_name, "SkyBox");
	}
	break;
	default:
		break;
	}
	if(new_object != nullptr)
	gfx->AddSceneObject(new_object, object_name);
	return new_object;
}

Drawable* GeometryFactory::GenerateGeometry(const char* file_name)
{
	assert(gfx != nullptr);
	Drawable* new_object = nullptr;
	new_object = new Model(*gfx,file_name);
	std::string name(file_name);
	char id[64];
	sprintf(id, "_%d", model_count++);
	std::string s_id(id);
	if (new_object != nullptr)
		gfx->AddSceneObject(new_object, (name+s_id).c_str());
	return new_object;
}

