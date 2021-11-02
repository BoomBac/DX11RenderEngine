#include <Public/Render/Bindable/TransformBuffer.h>

TransformBuffer::TransformBuffer(Graphics& gfx, Drawable& object):v_const_buf_(gfx),object_(object)
{

}

void TransformBuffer::Bind(Graphics& gfx)
{
	v_const_buf_.vc_buf_index_ = 1;// this->vc_buf_index_;
	v_const_buf_.Update(gfx, object_.GetTransform());
	v_const_buf_.Bind(gfx);
}

EBindableType TransformBuffer::GetType() const
{
	return EBindableType::kConstantBuffer;
}
