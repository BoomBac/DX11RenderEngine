#include "Public/Render/VertexLayout.h"

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
	static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	static constexpr const char* semantic = "Color";
	static constexpr const UINT offset = 12;
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
};  template<>
struct Map<VertexType::Float4Color>
{
	static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
	static constexpr const char* semantic = "Color";
	static constexpr const UINT offset = 16;
};




VertexLayout::VertexLayout()
{
}

D3D11_INPUT_ELEMENT_DESC* VertexLayout::Build()
{
	UINT offset = 0;
	// \是连接作用，连接不同行，表示下一行元素也是这一行
	//参数 x,body
	#define ITER_VERTEX_TYPE(x,body) if(i==VertexType::x){\
	using Map = Map<VertexType::x>;\
	body;\
	}
	for (const auto &i : v)
	{
		/*switch (i)
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
		}*/
		ITER_VERTEX_TYPE(Position2D, des.push_back(D3D11_INPUT_ELEMENT_DESC{ Map::semantic, 0, Map::dxgiFormat, 0, offset,
					  D3D11_INPUT_PER_VERTEX_DATA, 0 }); offset += Map::offset;);
		ITER_VERTEX_TYPE(Position3D, des.push_back(D3D11_INPUT_ELEMENT_DESC{ Map::semantic, 0, Map::dxgiFormat, 0, offset,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 }); offset += Map::offset;);
		ITER_VERTEX_TYPE(Float3Color, des.push_back(D3D11_INPUT_ELEMENT_DESC{ Map::semantic, 0, Map::dxgiFormat, 0, offset,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 }); offset += Map::offset;);
		ITER_VERTEX_TYPE(RGBAColor, des.push_back(D3D11_INPUT_ELEMENT_DESC{ Map::semantic, 0, Map::dxgiFormat, 0, offset,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 }); offset += Map::offset;);
		ITER_VERTEX_TYPE(Normal, des.push_back(D3D11_INPUT_ELEMENT_DESC{ Map::semantic, 0, Map::dxgiFormat, 0, offset,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 }); offset += Map::offset;);
		ITER_VERTEX_TYPE(Texture2D, des.push_back(D3D11_INPUT_ELEMENT_DESC{ Map::semantic, 0, Map::dxgiFormat, 0, offset,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 }); offset += Map::offset;);
		ITER_VERTEX_TYPE(Float4Color, des.push_back(D3D11_INPUT_ELEMENT_DESC{ Map::semantic, 0, Map::dxgiFormat, 0, offset,
			  D3D11_INPUT_PER_VERTEX_DATA, 0 }); offset += Map::offset;);

	}
	#undef 	ITER_VERTEX_TYPE;
	return &des.at(0);
}

UINT VertexLayout::GetItemNum() const
{
	return (UINT)v.size();
}

VertexLayout& VertexLayout::operator<<(VertexType type)
{
	v.push_back(type);
	return *this;
}