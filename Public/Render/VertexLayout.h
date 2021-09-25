#pragma once
#include <d3d11.h>
#include <vector>
#include "vector3D.h"

enum class VertexType : UINT
{
	Position2D,
	Position3D,
	Float3Color,
	Float4Color,
	RGBAColor,
	Normal,
	Texture2D,
};
struct Postion3DColored
{
	CusMath::vector3d pos;
	CusMath::vector3d color;
};


class VertexLayout
{
public:
	VertexLayout();
	D3D11_INPUT_ELEMENT_DESC* Build();
	VertexLayout& operator <<(VertexType type);
	UINT GetItemNum() const;
private:
	std::vector<VertexType> v;
	std::vector<D3D11_INPUT_ELEMENT_DESC> des;
};
