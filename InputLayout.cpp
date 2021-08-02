#include "InputLayout.h"
#include "VertexShader.h"
#include "VertexLayout.h"

InputLayout::InputLayout(Graphics& gfx, const VertexShader& vs, VertexLayout& vl)
{
	;
	//const D3D11_INPUT_ELEMENT_DESC layout[] =
	//{
	//	{"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	//	//{"Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
	//};
	//UINT numElements = ARRAYSIZE(layout);

	//创建输入布局
	GetDevice(gfx)->CreateInputLayout(vl.Build(), 1u,vs.pVSBlob->GetBufferPointer(),
		vs.pVSBlob->GetBufferSize(), pInputLayout.GetAddressOf());
}

void InputLayout::Bind(Graphics& gfx)
{
	GetContext(gfx)->IASetInputLayout(pInputLayout.Get());
	GetContext(gfx)->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

EBindableType InputLayout::GetType() const
{
	return EBindableType::InputLayout;
}
