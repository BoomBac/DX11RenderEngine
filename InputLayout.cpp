#include "InputLayout.h"
#include "VertexShader.h"

InputLayout::InputLayout(ID3D11Device* pd, const VertexShader& vs)
{
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		//{"Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	UINT numElements = ARRAYSIZE(layout);

	//创建输入布局
	pd->CreateInputLayout(layout, numElements,vs.pVSBlob->GetBufferPointer(),
		vs.pVSBlob->GetBufferSize(), pInputLayout.GetAddressOf());
}

void InputLayout::Bind(Graphics& gfx)
{

}

void InputLayout::TBind(ID3D11DeviceContext& pc)
{

}
