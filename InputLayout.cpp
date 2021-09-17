#include "InputLayout.h"
#include "VertexShader.h"
#include "VertexLayout.h"

InputLayout::InputLayout(Graphics& gfx, const VertexShader& vs, VertexLayout& vl)
{
	//创建输入布局
	GetDevice(gfx)->CreateInputLayout(vl.Build(), vl.GetItemNum(),vs.pVSBlob->GetBufferPointer(),
		vs.pVSBlob->GetBufferSize(), pInputLayout.GetAddressOf());
}

void InputLayout::Bind(Graphics& gfx)
{
	GetContext(gfx)->IASetInputLayout(pInputLayout.Get());
	GetContext(gfx)->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

EBindableType InputLayout::GetType() const
{
	return EBindableType::InputLayout;
}
