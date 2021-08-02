#include "TransformBuffer.h"

TransformBuffer::TransformBuffer(Graphics& gfx, Drawable& object):vbuf(gfx),object(object)
{

}

void TransformBuffer::Bind(Graphics& gfx)
{
	vbuf.Update(gfx, object.GetTransform());
	vbuf.Bind(gfx);
}

EBindableType TransformBuffer::GetType() const
{
	return EBindableType::ConstantBuffer;
}
