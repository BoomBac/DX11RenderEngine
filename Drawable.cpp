#include "Drawable.h"
#include "Graphics.h"
#include "Bindable.h"
#include "GraphicsResource.h"

//eyePos,lookAt,upDirection
DirectX::XMMATRIX Drawable::view = DirectX::XMMatrixLookAtLH({ 0.0f,50.0f, 0.0f, 0.0f },
	{ 0.0f,0.0f, 0.0f, 0.0f }, { 0.0f,0.0f, 1.0f, 0.0f });

DirectX::XMMATRIX Drawable::projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 4.0f / 3.0f, 0.01f, 100.f);

void Drawable::Draw(Graphics& gfx)
{
	for (auto& i : binds)
	{
		i->Bind(gfx);
	}
	for (auto& i : GetStaticBinds())
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
	indexbuffer = ibf.get();
	indexbuffer->Bind(gfx);
}

WorldTransform& Drawable::GetTransform()
{
	return transform;
}

void Drawable::UpdateCameraTransformation(const DirectX::XMMATRIX& tranf)
{
	view = tranf*view;
}



