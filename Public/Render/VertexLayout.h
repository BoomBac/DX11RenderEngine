#pragma once
#include <d3d11.h>
#include <vector>
#include "vector3D.h"
#include "vector2D.h"

enum class EVertexType : UINT
{
	kPosition2D,
	kPosition3D,
	kFloat3Color,
	kFloat4Color,
	kRGBAColor,
	kNormal,
	kTexture2D,
};
struct Postion3DColored
{
	CusMath::vector3d pos;
	CusMath::vector3d color;
};
struct Postion3DTN
{
	CusMath::vector3d pos;
	CusMath::vector2d uv;
	CusMath::vector3d normal;
};


class VertexLayout
{
public:
	VertexLayout();
	D3D11_INPUT_ELEMENT_DESC* Build();
	VertexLayout& operator <<(EVertexType type);
	UINT GetItemNum() const;
private:
	std::vector<EVertexType> v;
	std::vector<D3D11_INPUT_ELEMENT_DESC> des;
};

