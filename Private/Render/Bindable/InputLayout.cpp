#include <Public/Render/Bindable/InputLayout.h>

InputLayout::InputLayout(Graphics& gfx, const VertexShader& vs, VertexLayout& vl,D3D11_PRIMITIVE_TOPOLOGY topology)
{
	//创建输入布局
	GetDevice(gfx)->CreateInputLayout(vl.Build(), vl.GetItemNum(),vs.p_blob_->GetBufferPointer(),
		vs.p_blob_->GetBufferSize(), p_input_layout_.GetAddressOf());
	topology_ = topology;
}


void InputLayout::Bind(Graphics& gfx)
{
	GetContext(gfx)->IASetInputLayout(p_input_layout_.Get());
	GetContext(gfx)->IASetPrimitiveTopology(topology_);
}

EBindableType InputLayout::GetType() const
{
	return EBindableType::kInputLayout;
}
