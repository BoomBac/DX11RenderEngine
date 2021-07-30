#include "VertexLayout.h"


template<VertexType> struct Map;
template<> 
struct Map<VertexType::Position2D>
{
	static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
	static constexpr const char* semantic = "Position";
	static constexpr const UINT offset = 8;
};
struct Map<VertexType::Position3D>
{
	static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	static constexpr const char* semantic = "Position";
	static constexpr const UINT offset = 12;
};
struct Map<VertexType::Float3Color>
{
	static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
	static constexpr const char* semantic = "Color";
	static constexpr const UINT offset = 8;
};
struct Map<VertexType::RGBAColor>
{
	static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static constexpr const char* semantic = "Color";
	static constexpr const UINT offset = 4;
};
struct Map<VertexType::Normal>
{
	static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	static constexpr const char* semantic = "Normal";
	static constexpr const UINT offset = 12;
};
struct Map<VertexType::Texture2D>
{
	static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
	static constexpr const char* semantic = "Texcoord";
	static constexpr const UINT offset = 8;
};

VertexLayout::VertexLayout()
{
}

D3D11_INPUT_ELEMENT_DESC& VertexLayout::Build()
{
	UINT offset = 0;
	for (const auto &i : v)
	{
		switch (i)
		{
		case VertexType::Position2D:
		{
			des.push_back(
				D3D11_INPUT_ELEMENT_DESC{
					Map<VertexType::Position2D>::semantic,0,Map<VertexType::Position2D>::dxgiFormat,0,offset,
					D3D11_INPUT_PER_VERTEX_DATA,0

				});
			offset += ;
		}
		default:
			break;
		}
	}
}

VertexLayout& VertexLayout::operator<<(VertexType type)
{
	v.push_back(type);
	return *this;
}

//typedef struct D3D11_INPUT_ELEMENT_DESC
//{
//	LPCSTR SemanticName;
//	UINT SemanticIndex;
//	DXGI_FORMAT Format;
//	UINT InputSlot;
//	UINT AlignedByteOffset;
//	D3D11_INPUT_CLASSIFICATION InputSlotClass;
//	UINT InstanceDataStepRate;
//} 	D3D11_INPUT_ELEMENT_DESC;
//const D3D11_INPUT_ELEMENT_DESC layout[] =
//{
//	{"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
//	//{"Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
//};