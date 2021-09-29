#ifndef DX11ENGINE_RENDER_MESH_H
#define DX11ENGINE_RENDER_MESH_H

#include <vector>
#include <Public/Render/VertexLayout.h>

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

class Mesh
{
public:
	Mesh();
private:
	std::vector<Postion3DTN> g_vertices;
	std::vector<UINT> g_indices;
};


#endif //DX11ENGINE_RENDER_MESH_H
