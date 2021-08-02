#include "Drawable.h"
#include "Graphics.h"
#include "Bindable.h"
#include "GraphicsResource.h"


void Drawable::Draw(Graphics& gfx)
{
	for (auto& i : binds)
	{
		i->Bind(gfx);
	}
	gfx.DrawIndexed(indexbuffer->count);
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind)
{
	binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuf(std::unique_ptr<IndexBuffer> ibf, Graphics& gfx)
{
	indexbuffer = std::move(ibf);
	indexbuffer->Bind(gfx);
}

MVPTransform& Drawable::GetTransform()
{
	return transform;
}

