#include "VertexLayout.h"

template<VertexType> struct Map;
template<> 
struct Map<VertexType::Position2D>
{
	static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
	static constexpr const char* semantic = "Position";
	static constexpr const UINT offset = 8;
};
template<>
struct Map<VertexType::Position3D>
{
	static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	static constexpr const char* semantic = "Position";
    static constexpr const UINT offset = 12;
};
template<>
struct Map<VertexType::Float3Color>
{
	static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
	static constexpr const char* semantic = "Color";
	static constexpr const UINT offset = 8;
}; template<>
struct Map<VertexType::RGBAColor>
{
	static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static constexpr const char* semantic = "Color";
	static constexpr const UINT offset = 4;
}; template<>
struct Map<VertexType::Normal>
{
	static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	static constexpr const char* semantic = "Normal";
	static constexpr const UINT offset = 12;
}; template<>
struct Map<VertexType::Texture2D>
{
	static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
	static constexpr const char* semantic = "Texcoord";
	static constexpr const UINT offset = 8;
}; 


VertexLayout::VertexLayout()
{
}

D3D11_INPUT_ELEMENT_DESC* VertexLayout::Build()
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
			offset += Map<VertexType::Position2D>::offset;
		}
		break;
		case VertexType::Position3D:
		{
			des.push_back(
				D3D11_INPUT_ELEMENT_DESC{
					Map<VertexType::Position3D>::semantic,0,Map<VertexType::Position3D>::dxgiFormat,0,offset,
					D3D11_INPUT_PER_VERTEX_DATA,0

				});
			offset += Map<VertexType::Position3D>::offset;
		}
		break;
		case VertexType::RGBAColor:
		{
			des.push_back(
				D3D11_INPUT_ELEMENT_DESC{
					Map<VertexType::RGBAColor>::semantic,0,Map<VertexType::RGBAColor>::dxgiFormat,0,offset,
					D3D11_INPUT_PER_VERTEX_DATA,0

				});
			offset += Map<VertexType::RGBAColor>::offset;
		}
		break;
		case VertexType::Float3Color:
		{
			des.push_back(
				D3D11_INPUT_ELEMENT_DESC{
					Map<VertexType::Float3Color>::semantic,0,Map<VertexType::Float3Color>::dxgiFormat,0,offset,
					D3D11_INPUT_PER_VERTEX_DATA,0

				});
			offset += Map<VertexType::Float3Color>::offset;
		}
		break;
		default:
			break;
		}
	}
	return &des.at(0);
}

VertexLayout& VertexLayout::operator<<(VertexType type)
{
	v.push_back(type);
	return *this;
}