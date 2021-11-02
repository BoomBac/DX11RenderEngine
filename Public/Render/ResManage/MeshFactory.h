#ifndef DX11ENGINE_RENDER_RESMANAGE_MESHFACTORY_H
#define DX11ENGINE_RENDER_RESMANAGE_MESHFACTORY_H

#include <vector>
#include <Public/Render/VertexLayout.h>
#include <map>
#include <set>

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif

enum class EMeshType
{
	kNone,
	kUVAndNormal,
	kNormalOnly,
	kUVOnly
};

class MeshFactory
{
public:
	static MeshFactory& getInstance();
	bool AddMesh(std::string file_name);
	bool GetMesh(std::string file_name, std::vector<Postion3DTN2>** pv, std::vector<UINT>** pi);
private:
	MeshFactory() {};
	DISALLOW_COPY_AND_ASSIGN(MeshFactory)
	void CalculateBTN(const std::vector<UINT>& indices, std::vector<Postion3DTN2>& vertics);
	std::map<std::string, std::vector<Postion3DTN2>> vertices_pool_;
	std::map <std::string, std::vector<UINT>> indices_pool_;
	std::set<std::string> mesh_loaded_;
	void LoadMesh(std::string file_path, std::vector<Postion3DTN2>& vertics, std::vector<UINT>& indices);
};


#endif //DX11ENGINE_RENDER_RESMANAGE_MESHFACTORY_H