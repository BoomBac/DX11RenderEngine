
#ifndef DX11ENGINE_RENDER_RESMANAGE_PSHADERFACTORY_H
#define DX11ENGINE_RENDER_RESMANAGE_PSHADERFACTORY_H


#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;   \
    TypeName &operator=(const TypeName &) = delete;
#endif 

#include <Public/Render/Bindable/PixelShader.h>
#include <Public/Render/Bindable/VertexShader.h>

//class MeshFactory
//{
//public:
//	static MeshFactory& getInstance();
//	bool AddMesh(std::string file_path);
//	bool GetMesh(std::string file_path, std::vector<Postion3DTN2>** pv, std::vector<UINT>** pi);
//private:
//	MeshFactory() {};
//	DISALLOW_COPY_AND_ASSIGN(MeshFactory)
//		void CalculateBTN(const std::vector<UINT>& indices, std::vector<Postion3DTN2>& vertics);
//	std::map<std::string, std::vector<Postion3DTN2>> vertices_pool_;
//	std::map <std::string, std::vector<UINT>> indices_pool_;
//	std::set<std::string> mesh_loaded_;
//	void LoadMesh(std::string file_path, std::vector<Postion3DTN2>& vertics, std::vector<UINT>& indices);
//};

enum class EShaderType
{
    kPixelShader,
    kVertexShader
};

/// <summary>
/// Create and Load cso file
/// all file must be create in "Y:\Project_VS2019\DX11RenderEngine\Shaders\cso\"
/// </summary>
class ShaderFactory
{
public:
    static ShaderFactory& getInstance();
    void AddShader(std::string file_name, EShaderType type);
    std::shared_ptr<PixelShader> GetPShader(std::string file_name);
    std::shared_ptr<VertexShader> GetVShader(std::string file_name);

private:
    DISALLOW_COPY_AND_ASSIGN(ShaderFactory)
    ShaderFactory() {};
    std::map<std::string, std::shared_ptr<PixelShader>> pshader_lib_;
    std::map<std::string, std::shared_ptr<VertexShader>> vshader_lib_;
};

#endif//DX11ENGINE_RENDER_RESMANAGE_PSHADERFACTORY_H
